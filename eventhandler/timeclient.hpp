#ifndef EVENTHANDLER_TIMECLIENT_HPP_
#define EVENTHANDLER_TIMECLIENT_HPP_

#include "../other/global.h"

#include <arpa/inet.h>
#include <assert.h>

class TimeClient : public reactor::EventHandler
{
public:
    TimeClient() : EventHandler()
    {
        m_handle = socket(AF_INET, SOCK_STREAM, 0);
        assert(IsValidHandle(m_handle));
    }

    ~TimeClient()
    {
        close(m_handle);
    }

    bool ConnectServer(const char *ip, unsigned short port)
    {
        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = inet_addr(ip);
        if (connect(m_handle, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            ReportSocketError("connect");
            return false;
        }
        return true;
    }

    virtual reactor::handle_t GetHandle() const
    {
        return m_handle;
    }

    virtual void HandleRead()
    {
        memset(g_read_buffer, 0, BUFFERSIZE);
        int len = recv(m_handle, g_read_buffer, BUFFERSIZE, 0);
        if (len > 0)
        {
            fprintf(stderr, "%s", g_read_buffer);
            g_reactor.RegisterHandler(this, reactor::kWriteEvent);
        }
        else
        {
            ReportSocketError("recv");
        }
    }

    virtual void HandleWrite()
    {
        memset(g_write_buffer, 0, BUFFERSIZE);
        int len = sprintf(g_write_buffer, "time\r\n");
        len = send(m_handle, g_write_buffer, len, 0);
        if (len > 0)
        {
            fprintf(stderr, "%s", g_write_buffer);
            g_reactor.RegisterHandler(this, reactor::kReadEvent);
        }
        else
        {
            ReportSocketError("send");
        }
    }

    virtual void HandleError()
    {
        fprintf(stderr, "server closed\n");
        close(m_handle);
        exit(0);
    }

private:
    reactor::handle_t m_handle;
};
#endif