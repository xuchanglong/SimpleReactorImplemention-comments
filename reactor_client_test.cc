#include "test_common.hpp"
#include "timeclient.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char g_read_buffer[BUFFERSIZE];
char g_write_buffer[BUFFERSIZE];

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: %s ip port\n", argv[0]);
        return EXIT_FAILURE;
    }

    TimeClient client;
    if (!client.ConnectServer(argv[1], atoi(argv[2]))) {
        fprintf(stderr, "connect remote server failed\n");
        return EXIT_FAILURE;
    }

    g_reactor.RegisterHandler(&client, reactor::kWriteEvent);
    while (1) {
        g_reactor.HandleEvents();
        sleep(1);
    }
    g_reactor.RemoveHandler(&client);
    return EXIT_SUCCESS;
}
