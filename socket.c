/**
 * For maintaining socket-level setup and preparation for incoming requests. 
 * Manages zombie processes.
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "socket.h"

#define PORT "4080"
#define BACKLOG 20

int set_socket(void)
{
    struct addrinfo *ai = generate_addr();
    int optval = 1;
    int sockfd;
    int sockopt;
    int binding;

    sockfd = socket(ai->ai_family, SOCK_STREAM, ai->ai_protocol);
    if (sockfd == -1) return -1;

    sockopt = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval,
                         sizeof(optval));
    if (sockopt == -1) return -1;

    binding = bind(sockfd, ai->ai_addr, ai->ai_addrlen);
    if (binding == -1) {
        close(sockfd);
        return -1;
    }

    freeaddrinfo(ai);

    if (listen(sockfd, BACKLOG) == -1)
        return -1;
    return sockfd;
}

struct addrinfo *generate_addr(void)
{
    struct addrinfo hints;
    struct addrinfo *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    int status = getaddrinfo(NULL, PORT, &hints, &servinfo);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    return servinfo;
}

// ISBN: 978-1705309902
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void handle_sigchld(int s)
{
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = saved_errno;
}

void process_reaper(void)
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);

    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("Sigaction");
        exit(1);
    }
}