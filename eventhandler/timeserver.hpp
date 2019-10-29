#ifndef EVENTHANDLER_TIMESERVER_HPP_
#define EVENTHANDLER_TIMESERVER_HPP_

#include "../other/global.h"

#include <arpa/inet.h>

class RequestHandler : public reactor::EventHandler
{
public:
    RequestHandler(reactor::handle_t handle) : EventHandler(),
                                               m_handle(handle)
    {
    }

    virtual reactor::handle_t GetHandle() const
    {
        return m_handle;
    }

    virtual void HandleWrite()
    {
        struct tm *ttime;
        char now[64];
        time_t tt;

        memset(now, 0, 64);
        tt = time(NULL);
        ttime = localtime(&tt);
        strftime(now, 64, "%Y-%m-%d %H:%M:%S", ttime);

        memset(g_write_buffer, 0, sizeof(g_write_buffer));
        int len = sprintf(g_write_buffer, "current time: %s\r\n", now);
        len = send(m_handle, g_write_buffer, len, 0);
        if (len > 0)
        {
            fprintf(stderr, "send response to client, fd=%d\n", (int)m_handle);
            g_reactor.RegisterHandler(this, reactor::kReadEvent);
        }
        else
        {
            ReportSocketError("send");
        }
    }

    virtual void HandleRead()
    {
        memset(g_read_buffer, 0, sizeof(g_read_buffer));
        int len = recv(m_handle, g_read_buffer, BUFFERSIZE, 0);
        if (len > 0)
        {
            if (strncasecmp("time", g_read_buffer, 4) == 0)
            {
                g_reactor.RegisterHandler(this, reactor::kWriteEvent);
            }
            else if (strncasecmp("exit", g_read_buffer, 4) == 0)
            {
                close(m_handle);
                g_reactor.RemoveHandler(this);
                delete this;
            }
            else
            {
                fprintf(stderr, "Invalid request: %s", g_read_buffer);
                close(m_handle);
                g_reactor.RemoveHandler(this);
                delete this;
            }
        }
        else
        {
            ReportSocketError("recv");
        }
    }

    virtual void HandleError()
    {
        fprintf(stderr, "client %d closed\n", m_handle);
        close(m_handle);
        g_reactor.RemoveHandler(this);
        delete this;
    }

private:
    reactor::handle_t m_handle;
};

class TimeServer : public reactor::EventHandler
{
public:
    TimeServer(const char *ip, unsigned short port) : EventHandler(),
                                                      m_ip(ip),
                                                      m_port(port)
    {
    }

    /**
     * 启动服务器。
    */
    bool Start()
    {
        m_handle = socket(AF_INET, SOCK_STREAM, 0);
        if (!IsValidHandle(m_handle))
        {
            ReportSocketError("socket");
            return false;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(m_port);
        addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        if (bind(m_handle, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            ReportSocketError("bind");
            return false;
        }

        if (listen(m_handle, 10) < 0)
        {
            ReportSocketError("listen");
            return false;
        }
        return true;
    }

    /**
     * 返回监听 socket 描述符。
    */
    virtual reactor::handle_t GetHandle() const
    {
        return m_handle;
    }

    /**
     * 处理 server（事件处理器）的读事件。
     * 即：接收 client 发来的连接。
    */
    virtual void HandleRead()
    {
        struct sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        reactor::handle_t handle = accept(m_handle, &addr, &addrlen);
        if (!IsValidHandle(handle))
        {
            ReportSocketError("accept");
        }
        else
        {
            /**
             * 对该新连接注册读写事件。
            */
            RequestHandler *handler = new RequestHandler(handle);
            if (g_reactor.RegisterHandler(handler, reactor::kReadEvent) != 0)
            {
                fprintf(stderr, "error: register handler failed\n");
                delete handler;
            }
        }
    }

private:
    /**
     * 保存监听 socket 描述符。
     */
    reactor::handle_t m_handle;
    std::string m_ip;
    unsigned short m_port;
};

#endif