/**
 * Main thread for server runtime: from start to program exit. 
 * 
 * Primary run function is called upon program execution, with graceful shutdown
 * caused by signal. 
 */

#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "server.h"
#include "socket.h"
#include "hashmap.h"

static int runflag;

void signal_handler(int sig)
{
    runflag = -1;
}

void shutdown_server(thrdata_t *data)
{
    close(data->fd);

    process_reaper();
    
    cache_reclaim(data->cache);
}

void run_server(void)
{
    pthread_t threads;
    cache_t *cache;
    int sockfd;

    cache  = init_cache();
    sockfd = set_socket();

    thrdata_t init     = {cache, sockfd};
    thrdata_t *mainthr = &init;

    // Main thread, passing in cache and initial socket
    pthread_create(&threads, NULL, &accept_connect, mainthr);

    // End server run on CTRL+C
    signal(SIGINT, signal_handler);
    while (runflag !=-1) {
        sleep(1);
    }

    shutdown_server(mainthr);
}

int main(void)
{
    run_server();

    return EXIT_SUCCESS;
}