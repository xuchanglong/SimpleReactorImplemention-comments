#include <assert.h>
#include "reactor.h"
#include "../eventDemultiplexer/event_demultiplexer.h"

#define INITSIZE 100

namespace reactor
{
class ReactorImplementation
{
public:
    ReactorImplementation();
    ~ReactorImplementation();

public:
    /**
     * @function    将事件处理者和句柄绑定，并将句柄和事件放到 epoll 红黑树中。
     * @paras   handler 事件处理者。
     *          evt 待监控的事件，也是该事件处理者能够处理的事件。
     * @ret 0   操作成功。
    */
    int RegisterHandler(EventHandler *handler, event_t evt);

    int RemoveHandler(EventHandler *handler);

    /**
     * @function    调用各种事件处理器处理事件的函数。
     * @paras   none 。
     * @ret none 。
    */
    void HandleEvents();

    int RegisterTimerTask(heap_timer *timerevent);

private:
    EventDemultiplexer *m_demultiplexer;
    /**
     * 保存句柄和该事件处理者。
    */
    std::map<handle_t, EventHandler *> m_handlers;
    time_heap *m_eventtimer;
};

///////////////////////////////////////////////////////////////////////////////

Reactor::Reactor()
{
    m_reactor_impl = new ReactorImplementation();
}

Reactor::~Reactor()
{
    delete m_reactor_impl;
}

int Reactor::RegisterHandler(EventHandler *handler, event_t evt)
{
    return m_reactor_impl->RegisterHandler(handler, evt);
}

int Reactor::RemoveHandler(EventHandler *handler)
{
    return m_reactor_impl->RemoveHandler(handler);
}

void Reactor::HandleEvents()
{
    m_reactor_impl->HandleEvents();
}

int Reactor::RegisterTimerTask(heap_timer *timerevent)
{
    return m_reactor_impl->RegisterTimerTask(timerevent);
}

///////////////////////////////////////////////////////////////////////////////

ReactorImplementation::ReactorImplementation()
{
    m_demultiplexer = new EpollDemultiplexer();
    m_eventtimer = new time_heap(INITSIZE);
}

ReactorImplementation::~ReactorImplementation()
{
    delete m_demultiplexer;
}

int ReactorImplementation::RegisterHandler(EventHandler *handler, event_t evt)
{
    handle_t handle = handler->GetHandle();
    std::map<handle_t, EventHandler *>::iterator it = m_handlers.find(handle);
    if (it == m_handlers.end())
    {
        m_handlers[handle] = handler;
    }
    return m_demultiplexer->RequestEvent(handle, evt);
}

int ReactorImplementation::RemoveHandler(EventHandler *handler)
{
    handle_t handle = handler->GetHandle();
    m_handlers.erase(handle);
    return m_demultiplexer->UnrequestEvent(handle);
}

//parm timeout is useless.
void ReactorImplementation::HandleEvents()
{
    int timeout = 0;
    if (m_eventtimer->top() == NULL)
    {
        timeout = 0;
    }
    else
    {
        timeout = ((m_eventtimer->top())->expire - time(NULL)) * 1000;
    }
    m_demultiplexer->WaitEvents(&m_handlers, timeout, m_eventtimer);
}

int ReactorImplementation::RegisterTimerTask(heap_timer *timerevent)
{
    if (timerevent == NULL)
        return -1;
    m_eventtimer->add_timer(timerevent);
    return 0;
}

} // namespace reactor
