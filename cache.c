/**
 * Cache operates in a Least Recently Used fashion, constructed as a
 * doubly linked list (doubly.c) with each node stored in a hashmap. 
 * 
 * The cache memory is intended to be allocated for the runtime of the server.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cache.h"
#include "doubly.h"
#include "hashmap.h"

#define CACHE_CAP 53

int cache_check(cache_t *cache, char *URL)
{
    if (URL == NULL) {
        return -1;
    }
    int key = hm_index(URL);

    if (cache->buckets[key] != NULL) {
        return 1;
    }
    return 0;
}

void cache_add(cache_t *cache, char *URL, char *body)
{
    int key;
    node_t *node;

    if (cache->size == CACHE_CAP) {
        cache_remove(cache);
    }
    key = hm_index(URL);
    
    // Update doubly list
    node = create_node(key, body);
    dll_push(&(cache->dll_head), node);

    // Update hashmap
    hm_put(cache->buckets, key, node);

    cache->size++;
}

char * cache_get(cache_t *cache, char *str)
{
    int key = hm_index(str);

    if (key >= 0) {
        return hm_get(cache->buckets, key);
    }
    return NULL;
}

void cache_remove(cache_t *cache)
{
    int key; 

    cache->size--;
    key = dll_pop(cache->dll_tail);
    
    cache->buckets[key] = NULL;           
}

cache_t * init_cache(void)
{
    cache_t *cache = malloc(sizeof(cache_t));

    cache->size = 0;
    cache->buckets = init_hm();
    
    cache->dll_head = create_node(-1, "HEAD - CANNOT MOVE");
    cache->dll_tail = create_node(-1, "TAIL - CANNOT MOVE");

    cache->dll_head->next = cache->dll_tail;
    cache->dll_head->prev = NULL;
    cache->dll_tail->next = NULL;
    cache->dll_tail->prev = cache->dll_head;

    return cache;
}

void cache_reclaim(cache_t *cache)
{
    node_reclaim(cache->dll_head);
    hm_reclaim(cache->buckets);

    free(cache);
}