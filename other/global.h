#ifndef OTHER_GLOBAL_H_
#define OTHER_GLOBAL_H_

#include "../reactor/reactor.h"
#include "singleton.h"

//reactor::Reactor g_reactor;
#define g_reactor (*(sGlobal->g_reactor_ptr))

#define BUFFERSIZE 1024
extern char g_read_buffer[BUFFERSIZE];
extern char g_write_buffer[BUFFERSIZE];

//class reactor::Reactor;

class Global : public Singleton<Global>
{
public:
    Global(void);
    ~Global(void);

    reactor::Reactor *g_reactor_ptr;
};

#define sGlobal Global::instance()

#endif
