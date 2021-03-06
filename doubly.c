/**
 * Doubly linked list, storing a node_t containing a key:value combination.
 * The key correlates to the hashmap(.c) entry, and assist removal from the
 * hashmap upon node_t deletion.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "doubly.h"

void dll_push(node_t **head, node_t *node)
{
    node->prev = *head;
    node->next = (*head)->next;
    
    (*head)->next->prev = node;
    (*head)->next = node;
}

int dll_pop(node_t *tail)
{
    int key;

    node_t *temp = tail->prev->prev;
    temp->next = tail;

    key = tail->prev->key;
    free(tail->prev);
    tail->prev = temp;

    return key;
}

void dll_appendtohead(node_t **head, node_t *node)
{
    if (node->prev == NULL) {
        return;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;

    dll_push(head, node);
    free(node);
}

node_t * create_node(int key, char *value)
{
    node_t *node = malloc(sizeof(node_t));
    node->value  = malloc((strlen(value) + 1) * sizeof(char));

    node->key   = key;
    memcpy(node->value, value, strlen(value));
    node->prev  = NULL;
    node->next  = NULL;

    return node;
}

void node_reclaim(node_t *head)
{
    while (head != NULL) {
        node_t *temp = head;
        head = head->next;
        
        free(temp->value);
        temp->next = NULL;
        temp->prev = NULL;
        free(temp);
    }
}