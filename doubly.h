#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/** Contains key:value pair for hashmap and linkedlist configuration. */
typedef struct node {
    int         key;
    char        *value;
    struct node *prev;
    struct node *next;	
} node_t;

node_t * create_node(int key, char *value);
void dll_push(node_t **head, node_t *node);
int dll_pop(node_t *tail);
void dll_appendtohead(node_t **head, node_t *node);
void node_reclaim(node_t *node);

#endif