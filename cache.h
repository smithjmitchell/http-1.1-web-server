#ifndef CACHE_H
#define CACHE_H

#include "doubly.h"

/**
 * Contains current size of the cache, pointer to hashmap buckets, and a 
 * the head and tail doubly linked list nodes that remain constant.
 */
typedef struct cache {
    int    size;
    node_t **buckets;
    node_t *dll_head;
    node_t *dll_tail;
} cache_t;

cache_t * init_cache(void);
void cache_add(cache_t *cache, char *URL, char *body);
char * cache_get(cache_t *cache, char *str);
void cache_remove(cache_t *cache);
int cache_check(cache_t *cache, char *str);
void cache_reclaim(cache_t *cache);

#endif