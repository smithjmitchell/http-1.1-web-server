#ifndef HASHMAP_H
#define HASHMAP_H

int hm_index(char *str);
void hm_put(node_t *buckets[], int key, node_t *temp);
char * hm_get(node_t *buckets[], int key);
node_t ** init_hm(void);
void hm_reclaim(node_t **buckets);

#endif