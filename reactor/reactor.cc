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
     * @function    将事件处理者信息注册到 Reactor 中。
     * @paras   handler 事件处理者。
     *          evt 待监控的事件，也是该事件处理者能够处理的事件。
     * @ret 0   操作成功。
     *      -1  形参为空。
     *      其他    操作失败。
    */
    int RegisterHandler(EventHandler *handler, event_t evt);

    /**
     * @function    将该事件处理者信息从 Rector 中删除。
     * @paras   hanlder 待删除的事件处理者。
     * @ret 0   操作成功
     *      -1  形参为空。
     *      其他    操作失败。
    */
    int RemoveHandler(EventHandler *handler);

    /**
     * @function    监控事件的到来并分发事件到各个事件处理者。
     * @paras   none 。
     * @ret none 。
    */
    void HandleEvents();

    int RegisterTimerTask(heap_timer *timerevent);

private:
    /**
     * 工厂模式，指向具体的多路事件分配器。
     * 现在指向的是基于 epoll 的多路事件分配器，
     * 将来可以使用 select、poll 等其他的多路分配器。
    */
    EventDemultiplexer *m_demultiplexer;

    /**
     * 保存句柄和该事件处理者的映射关系。
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
    m_demultiplexer = nullptr;
}

int ReactorImplementation::RegisterHandler(EventHandler *handler, event_t evt)
{
    if (handler == nullptr)
    {
        return -1;
    }
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
    if (handler == nullptr)
    {
        return -1;
    }

    handle_t handle = handler->GetHandle();
    m_handlers.erase(handle);
    return m_demultiplexer->UnrequestEvent(handle);
}

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
    if (timerevent == nullptr)
    {
        return -1;
    }
    m_eventtimer->add_timer(timerevent);
    return 0;
}

} // namespace reactor
