#ifndef SERVER_H
#define SERVER_H

#include "cache.h"

/** Holds and collects all required information from incoming request. */
typedef struct {
    int   fd;
    char  method[40]; 
    char  path[40];
    int   version;
    char  body[400000];
    int   bodylen;
    char *headers;
    char  gmttime[100];
    char  code[40];
} request_t;

/** 
 * Holds a reference to the cache (shared between all threads) and an
 * individual file descriptor returned from accept().
 */
typedef struct {
        cache_t *cache;
        int fd;
} thrdata_t;

// Socket level
void * accept_connect(void *info);

// Request / response
void send_error_response(char *err, char *reason, request_t *request);
void * receive_request(void *data);
void parse_request(cache_t *cache, char *buffer, request_t *request);
void send_response(request_t *request);
int parse_headers(char *received, request_t *request);
void append_headers(char *response, char *buffer, request_t *request);
int determine_body(cache_t *cache, request_t *request, char *URL);

// Helper
int is_valid_method(const char *method);
char * gmt_datetime(char *buff);
void handle_sig(int sig);

#endif