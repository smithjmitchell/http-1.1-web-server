/**
 * Hashmap stores key:value pairs for url:response_body combinations. 
 * 
 * The hash calculation is "straightforward" and may result in collisions, 
 * noting the smaller size of the HASH_CAP.
 */

#include <stdio.h>
#include <stdlib.h>

#include "doubly.h"
#include "hashmap.h"

#define HASH_CAP 163

int hm_index(char *str)
{
    size_t hashval; 

    for (hashval = 0; *str != 0; str++) {
        hashval = *str + 31*hashval;
    }
    return hashval % HASH_CAP;
}

void hm_put(node_t **buckets, int key, node_t *node)
{
    node->key = key;
    buckets[key] = node;
}

char * hm_get(node_t **buckets, int key)
{
    if (buckets[key] == NULL) {
        return NULL;
    }
    return buckets[key]->value;
}

node_t ** init_hm(void)
{
    int i;
    
    node_t **buckets = malloc(HASH_CAP * sizeof(node_t *));
    
    for (i = 0; i < HASH_CAP; i++) {
        buckets[i] = NULL;
    }
    return buckets;
}

void hm_reclaim(node_t **buckets)
{
    free(buckets);
}