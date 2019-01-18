#include <stdlib.h>
#include <math.h>

#include "std/priority_queue.h"

/**
 *  Crea un nuovo nodo di una coda a prorità
 *
 *  @param key Priorità con la quale va creato il nodo
 *  @param value Valore del nodo
 *
 *  @return Nodo
 */
priority_queue_node_t * priority_queue_node_new(int key, void * value)
{

    priority_queue_node_t * node = memalloc(priority_queue_node_t);

    node->key = key;
    node->value = value;

    return node;

}

/**
 *  Dealloca un nodo di una coda a prorità
 *
 *  @param node Nodo
 */
void priority_queue_node_delete(priority_queue_node_t * node)
{

    memfree(node);

}

/**
 *  Scambia due nodi della coda
 *
 *  @param pqueue Coda
 *  @param i Indice del primo nodo
 *  @param j Indice del secondo nodo
 */
void priority_queue_nodes_swap(priority_queue_t * pqueue, int i, int j)
{

    priority_queue_node_t * node = pqueue->nodes[i];
    pqueue->nodes[i] = pqueue->nodes[j];
    pqueue->nodes[j] = node;

    pqueue->nodes[i]->index = i;
    pqueue->nodes[j]->index = j;

}

priority_queue_t * priority_queue_new(int type, int size)
{

    priority_queue_t * queue = memalloc(priority_queue_t);

    queue->type = (type == PRIORITY_QUEUE_MIN) ? PRIORITY_QUEUE_MIN : PRIORITY_QUEUE_MAX;

    queue->nodes = memalloc(priority_queue_node_t *, size, 1);
    queue->size = size;
    queue->length = 0;

    return queue;

}

void priority_queue_delete(priority_queue_t * pqueue)
{

    //  1. deallocazione nodi
    int i;
    for (i = 0; i < pqueue->length; i++) {
        priority_queue_node_delete(pqueue->nodes[i]);
    }

    memfree(pqueue->nodes);

    //  2. deallocazione coda
    memfree(pqueue);

}

/**
 *  Ripristina la proprietà di max-heap
 *
 *  @param pqueue Coda
 *  @param index Indice dal quale effettuare i controlli
 */
void priority_queue_max_heapify(priority_queue_t * pqueue, int index)
{

    int left = priority_queue_left(index);
    int right = priority_queue_right(index);

    int max;

    if (left < pqueue->length && pqueue->nodes[left]->key > pqueue->nodes[index]->key) {
        max = left;
    } else {
        max = index;
    }

    if (right < pqueue->length && pqueue->nodes[right]->key > pqueue->nodes[max]->key)
        max = right;

    if (max != index) {
        priority_queue_nodes_swap(pqueue, index, max);
        priority_queue_max_heapify(pqueue, max);
    }

}

/**
 *  Ripristina la proprietà di min-heap
 *
 *  @param pqueue Coda
 *  @param index Indice dal quale effettuare i controlli
 */
void priority_queue_min_heapify(priority_queue_t * pqueue, int index)
{

    int left = priority_queue_left(index);
    int right = priority_queue_right(index);

    int max;

    if (left < pqueue->length && pqueue->nodes[left]->key < pqueue->nodes[index]->key) {
        max = left;
    } else {
        max = index;
    }

    if (right < pqueue->length && pqueue->nodes[right]->key < pqueue->nodes[max]->key)
        max = right;

    if (max != index) {
        priority_queue_nodes_swap(pqueue, index, max);
        priority_queue_max_heapify(pqueue, max);
    }

}

void priority_queue_decrease_key(priority_queue_t * pqueue, priority_queue_node_t * node, priority_queue_key_t key)
{

    int index = node->index;

    //  la nuova chiave è troppo grande
    if (key > pqueue->nodes[index]->key)
        return;

    pqueue->nodes[index]->key = key;

    while (index > 1 && pqueue->nodes[priority_queue_parent(index)]->key > pqueue->nodes[index]->key) {

        int parent = priority_queue_parent(index);

        priority_queue_nodes_swap(pqueue, parent, index);
        index = parent;

    }

}

void priority_queue_increase_key(priority_queue_t * pqueue, priority_queue_node_t * node, priority_queue_key_t key)
{

    int index = node->index;

    //  la nuova chiave è troppo piccola
    if (key < pqueue->nodes[index]->key)
        return;

    pqueue->nodes[index]->key = key;

    while (index > 1 && pqueue->nodes[priority_queue_parent(index)]->key < pqueue->nodes[index]->key) {

        int parent = priority_queue_parent(index);

        priority_queue_nodes_swap(pqueue, parent, index);
        index = parent;

    }

}

priority_queue_node_t * priority_queue_insert(priority_queue_t * pqueue, priority_queue_key_t key, void * value)
{

    //  non c'è spazio per nuovi elementi
    if (pqueue->length >= pqueue->size) {
        return NULL;
    }

    //  indice dell'elemento
    int index = pqueue->length++;

    //  creazione del nodo
    priority_queue_node_t * node = priority_queue_node_new((pqueue->type == PRIORITY_QUEUE_MIN ? -INFINITY : INFINITY), value);
    node->index = index;

    //  inserimento
    pqueue->nodes[index] = node;

    //  ripristino della proprietà di heap
    if (pqueue->type == PRIORITY_QUEUE_MIN) {
        priority_queue_decrease_key(pqueue, node, key);
    } else {
        priority_queue_increase_key(pqueue, node, key);
    }

    return node;

}

void * priority_queue_extract(priority_queue_t * pqueue)
{

    //  non c'è niente da estrarre
    if (pqueue->length <= 0)
        return NULL;

    void * value = pqueue->nodes[0]->value;

    //  si scambia la prima con l'ultima chiave
    priority_queue_nodes_swap(pqueue, 0, pqueue->length - 1);
    priority_queue_node_delete(pqueue->nodes[--pqueue->length]);

    if (pqueue->type == PRIORITY_QUEUE_MIN)
        priority_queue_min_heapify(pqueue, 0);
    else
        priority_queue_max_heapify(pqueue, 0);

    return value;

}

void * priority_queue_extract_min(priority_queue_t * pqueue)
{

    //  è un max heap
    if (pqueue->type == PRIORITY_QUEUE_MAX)
        return NULL;

    return priority_queue_extract(pqueue);

}

void * priority_queue_extract_max(priority_queue_t * pqueue)
{

    //  è un min heap
    if (pqueue->type == PRIORITY_QUEUE_MIN)
        return NULL;

    return priority_queue_extract(pqueue);

}

void * priority_queue_head(priority_queue_t * pqueue)
{

    if (pqueue->length > 0)
        return pqueue->nodes[0]->value;

    return NULL;

}
