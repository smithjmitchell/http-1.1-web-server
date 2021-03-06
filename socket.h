#ifndef SOCKET_H
#define SOCKET_H

// Socket configuration
int set_socket(void);
struct addrinfo * generate_addr(void);
void * get_in_addr(struct sockaddr *sa);

// Processes
void handle_sigchld(int s);
void process_reaper(void);

#endif