/**
 * Server logic for accepting incoming request(s), parsing the request,
 * ensuring basic standards are met for HTTP/1.#, handling errors, and sending 
 * the appropriate response(s).
 * 
 * Additionally, logic for concurrency and shared resources is within. 
 */

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "cache.h"
#include "hashmap.h"
#include "server.h"
#include "socket.h"

#define RUN         1
#define BUFFER      400000
#define FILE_ROOT   "./serverfiles/"

void * accept_connect(void *info)
{
    int new_fd;
    struct sockaddr_storage addr;
    socklen_t addr_size = sizeof(addr);

    // Threading variables
    pthread_t threads[10];
    int rc;
    long thr = 0;

    // Contains cache and initial socket
    thrdata_t * main = info;
        
    while (RUN) {
        new_fd = accept(main->fd, (struct sockaddr *) &addr, &addr_size);
        if (new_fd == -1) {
            perror("Accept");
            break;
        }

        // Thread struct, passing in original cache and new socket
        thrdata_t init = {main->cache, new_fd};
        thrdata_t *newthr = &init;

        rc = pthread_create(&threads[thr++], NULL, receive_request, newthr);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }
}

void * receive_request(void *data)
{
    int bytes;
    char buffer[BUFFER];

    char received[BUFFER] = "\0";
    thrdata_t * newthr    = data;

    // Request struct
    request_t  init    = {0,'\0','\0',0,'\0',0,'\0','\0','\0'};
    request_t *request = &init;
    request->fd        = newthr->fd;

    while (RUN) {
        bytes = recv(request->fd, buffer, BUFFER, 0);
        if (bytes == -1) {
            perror("Recv");
            return NULL;
        }

        buffer[bytes] = '\0';
        if ((bytes == 2) && strstr(buffer, "\r\n")) {
            break;
        }
        strcat(received, buffer);
    }
    parse_request(newthr->cache, received, request);

    close(request->fd);
    pthread_exit(NULL);
}

void parse_request(cache_t *cache, char *received, request_t *request)
{
    FILE *stream;
    char URL[40];

    sscanf(received, "%s %s HTTP/1.%d", request->method, URL, 
           &(request->version));

    strcat(request->path, "./serverfiles/");
    strcat(request->path, URL);
    
    if (!is_valid_method(request->method)) {
        send_error_response("501", "Not Implemented", request);
        return;
    }

    if (parse_headers(received, request) == -1)    return;
    if (determine_body(cache, request, URL) == -1) return;

    // No errors? Good request
    if (request->code == NULL) {
        strcat(request->code, "200 OK");
    }
    request->bodylen = strlen(request->body);

    send_response(request);
}

/**
 * If file is not found in cache, add it. If bad request, send error.
 */
int determine_body(cache_t *cache, request_t *request, char *URL)
{
    FILE *stream;
    int check = cache_check(cache, URL);

    switch(check) {
        case 1:
            strcpy(request->body, cache_get(cache, URL));
            return 1;

        case 0: 
            stream = fopen(request->path, "r");

            if (stream == NULL) {
                send_error_response("404", "Not Found", request);
                return -1;
            }
            
            fread(request->body, sizeof(char), BUFFER, stream);
            cache_add(cache, URL, request->body);
            return 0;

        default:
            return -1;
    }
}

int is_valid_method(const char *method) 
{
    return !strcmp(method, "GET") || !strcmp(method, "HEAD");
}

int parse_headers(char *received, request_t *request)
{
    if (!strstr(received, "Host:") && request->version == 1) {
        send_error_response("400", "Bad Request", request);
        return -1;
    }
    
    if (strstr(received, "Expect: 100-continue")) {
        send(request->fd, "HTTP/1.1 100 Continue\r\n\r\n", 25, 0);
    }

    return 0;
}

void send_error_response(char *code, char *reason, request_t *request)
{
    char error[12] = "Error: ";
    
    strcat(error, code);
    strcat(request->code, error);
    strcat(request->body, reason);

    send_response(request);
}

void send_response(request_t *request)
{
    char time[30];
    char response[2000] = {0};
    char buffer[BUFFER] = {0};

    // Lead
    snprintf(buffer, BUFFER, "HTTP/1.%d %s\r\n", request->version, request->code);
    strcat(response, buffer);

    // Date
    snprintf(buffer, BUFFER, "%s %s\r\n", "Date:", gmt_datetime(time));
    strcat(response, buffer);

    // Headers
    append_headers(response, buffer, request);

    // Body
    if (strcmp(request->method, "HEAD") != 0) {
        strcat(response, "\r\n");
        strcat(response, request->body);
        strcat(response, "\r\n");
    }

    if (send(request->fd, response, strlen(response), 0) == -1) {
        printf("LOG: Sending error.\n");
    }
}

void append_headers(char *response, char *buffer, request_t *request)
{
    snprintf(buffer, BUFFER, "Server: msmithgit/0.1a-dev\r\n");
    strcat(response, buffer);

    if (request->bodylen > 0) {
        snprintf(buffer, BUFFER, "Content-Length: %d\r\n", request->bodylen);
        strcat(response, buffer);
        snprintf(buffer, BUFFER, "Content-Type: text/html\r\n");
        strcat(response, buffer);
    }
}

char * gmt_datetime(char *buff) 
{
    struct tm *gmt;
    time_t rawtime;

    time(&rawtime);
    gmt = gmtime(&rawtime);

    strftime(buff, 30, "%a, %d %b %Y %H:%M:%S GMT", gmt);
    return buff;
}