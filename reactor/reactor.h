#ifndef REACTOR_REACTOR_H_
#define REACTOR_REACTOR_H_

#include "singleton.hpp"
#include "timeheap.hpp"

#include <stdint.h>
#include <unistd.h>
#include <sys/epoll.h>

namespace reactor {
typedef unsigned int event_t;
enum {
    kReadEvent = 0x01,
    kWriteEvent = 0x02,
    kErrorEvent = 0x04,
    kEventMask = 0xff
};

typedef int handle_t;

/**
 * 抽象类，事件处理者。
 * 所有的需要处理事件的类都继承于它。
*/
class EventHandler {
protected:
    EventHandler() {}
    virtual ~EventHandler() {}

public:
    /**
     * 返回该事件处理者的句柄。
    */
    virtual handle_t GetHandle() const = 0;

    /**
     * 处理读事件的接口。
    */
    virtual void HandleRead() {}

    /**
     * 处理写事件的接口。
    */
    virtual void HandleWrite() {}

    /**
     * 处理错误事件的接口。
    */
    virtual void HandleError() {}
};

class ReactorImplementation;

/**
 * 规范了用于事件调度的方式，提供了对事件处理器的注册、删除等设施。
 * 是整个 Reactor 模式的核心所在。
*/
class Reactor {
public:
    Reactor();
    ~Reactor();

public:
    int RegisterHandler(EventHandler *handler, event_t evt);

    int RemoveHandler(EventHandler *handler);

    void HandleEvents();

    int RegisterTimerTask(heap_timer *timerevent);

private:
    Reactor(const Reactor &);
    Reactor &operator=(const Reactor &);

private:
    ReactorImplementation *m_reactor_impl;
};

} // namespace reactor
#endif // REACTOR_REACTOR_H_
