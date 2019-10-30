#ifndef REACTOR_EVENT_DEMULTIPLEXER_H_
#define REACTOR_EVENT_DEMULTIPLEXER_H_

#include <set>
#include <map>
#include "reactor.h"

namespace reactor
{
/**
 * 抽象类，事件多路分配器。
*/
class EventDemultiplexer
{
public:
    virtual ~EventDemultiplexer() {}

    virtual int WaitEvents(std::map<handle_t, EventHandler *> *handlers,
                           int timeout = 0, time_heap *event_timer = nullptr) = 0;

    virtual int RequestEvent(handle_t handle, event_t evt) = 0;

    virtual int UnrequestEvent(handle_t handle) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * 基于 epoll 的事件多路分配器。
*/
class EpollDemultiplexer : public EventDemultiplexer
{
public:
    EpollDemultiplexer();

    ~EpollDemultiplexer();

    /**
     * @function    分发事件给事件处理器。
     * @paras   handlers    保存了句柄和事件处理器的映射关系。
     *          timeout epoll 中监控事件来与否的超时时间。
     *          event_timer 后续补充。
     * @ret 实际发生了事件的句柄的数量。
    */
    virtual int WaitEvents(std::map<handle_t, EventHandler *> *handlers,
                           int timeout = 0, time_heap *event_timer = nullptr);

    /**
     * @function    向 epoll 红黑树中修改/增加指定的句柄和事件。
     * @paras   handle  句柄。
     *          evt 事件。
     * @ret 0   成功。
    */
    virtual int RequestEvent(handle_t handle, event_t evt);

    /**
     * @function    从 epoll 红黑树中删除指定的 socket 对应的要监控的事件。
     * @paras   handle  socket 描述符。
     * @ret 0   操作成功
     *      非0 操作失败。
    */
    virtual int UnrequestEvent(handle_t handle);

private:
    /**
     * epoll 对象描述符。
    */
    int m_epoll_fd;

    /**
     * epoll 红黑树中节点的数量。
    */
    int m_fd_num;
};
} // namespace reactor

#endif // REACTOR_EVENT_DEMULTIPLEXER_H_
