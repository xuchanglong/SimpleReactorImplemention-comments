#ifndef REACTOR_TEST_COMMON_H_
#define REACTOR_TEST_COMMON_H_

#include "../reactor/reactor.h"

#include <string.h>

bool IsValidHandle(reactor::handle_t handle)
{
    return handle >= 0;
}

void ReportSocketError(const char *msg)
{
    fprintf(stderr, "%s error: %s\n", msg, strerror(errno));
}
#endif // REACTOR_TEST_COMMON_H_
