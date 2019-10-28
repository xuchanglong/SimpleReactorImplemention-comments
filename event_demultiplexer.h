#ifndef REACTOR_EVENT_DEMULTIPLEXER_H_
#define REACTOR_EVENT_DEMULTIPLEXER_H_

#include <set>
#include <map>
#include "reactor.h"

namespace reactor
{
/**
 * 接口类，事件多路分配器。
*/
class EventDemultiplexer
{
public:
    virtual ~EventDemultiplexer() {}

    /**
     * @function    分发事件的函数。
     * @paras   后续补充。
     * @ret 后续补充。
    */
    virtual int WaitEvents(std::map<handle_t, EventHandler *> *handlers,
                           int timeout = 0, time_heap *event_timer = NULL) = 0;

    virtual int RequestEvent(handle_t handle, event_t evt) = 0;

    virtual int UnrequestEvent(handle_t handle) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * 事件多路分配器的 epoll 实现类。
*/
class EpollDemultiplexer : public EventDemultiplexer
{
public:
    EpollDemultiplexer();

    ~EpollDemultiplexer();

    virtual int WaitEvents(std::map<handle_t, EventHandler *> *handlers,
                           int timeout = 0, time_heap *event_timer = NULL);

    virtual int RequestEvent(handle_t handle, event_t evt);

    virtual int UnrequestEvent(handle_t handle);

private:
    /**
     * epoll 对象描述符。
    */
    int m_epoll_fd;
    int m_fd_num;
};
} // namespace reactor

#endif // REACTOR_EVENT_DEMULTIPLEXER_H_
