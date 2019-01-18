#ifndef std_priority_queue_h
#define std_priority_queue_h

#include "types.h"

/** Tipo delle chiavi */
typedef float priority_queue_key_t;

/**
 *  Tipo di coda
 */
enum {
    PRIORITY_QUEUE_MIN,
    PRIORITY_QUEUE_MAX
};

/**
 *  Nodo di una coda a priorità
 */
typedef struct priority_queue_node_s {

    /** Chiave intera che ne determina la posizione */
    priority_queue_key_t key;

    /** Valore */
    void * value;

    /** Indice nell'array */
    int index;

} priority_queue_node_t;

/**
 *  Coda a priorità
 */
typedef struct priority_queue_s {

    /** Nodi della coda */
    priority_queue_node_t ** nodes;

    /** Numero massimo di elementi nella coda */
    int size;

    /** Numero di elementi attualmente nella coda */
    int length;

    /** Tipo di coda */
    int type;

} priority_queue_t;

/**
 *  Crea una nuova coda a prorità
 *
 *  @param type Tipo di coda
 *  @param size Dimensione della coda
 *
 *  @return Coda
 */
priority_queue_t * priority_queue_new(int type, int size);

/**
 *  Dealloca una coda a prorità
 *
 *  @param pqueue Coda
 */
void priority_queue_delete(priority_queue_t * pqueue);

/**
 *  Inserisce un nuovo elemento in una coda a priorità
 *
 *  @param pqueue Coda
 *  @param key Chiave dell'elemento
 *  @param value Valore dell'elemento
 *
 *  @return Nodo
 */
priority_queue_node_t * priority_queue_insert(priority_queue_t * pqueue, priority_queue_key_t key, void * value);

/**
 *  Legge il valore dell'elemento chiave minima se min heap,
 *  Legge il valore dell'elemento chiave massima se max heap
 *
 *  @param pqueue Coda
 *
 *  @return Valore dell'elemento con chiave minima/massima
 */
void * priority_queue_head(priority_queue_t * pqueue);

/**
 *  Estra l'elemento con valore della chiave minima (se min heap)
 *
 *  @param pqueue Coda
 *
 *  @return Valore dell'elemento con chiave minima
 */
void * priority_queue_extract_min(priority_queue_t * pqueue);

/**
 *  Estra l'elemento con valore della chiave massima (se max heap)
 *
 *  @param pqueue Coda
 *
 *  @return Valore dell'elemento con chiave massima
 */
void * priority_queue_extract_max(priority_queue_t * pqueue);

/**
 *  Riduce il valore della chiave di un elemento di una coda a priorità e ne ripristina l'ordine
 *
 *  @param pqueue Coda
 *  @param node Nodo del quale aggiornare la chiave
 *  @param key Nuovo valore della chiave
 */
void priority_queue_decrease_key(priority_queue_t * pqueue, priority_queue_node_t * node, priority_queue_key_t key);

/**
 *  Aumenta il valore della chiave di un elemento di una coda a priorità e ne ripristina l'ordine
 *
 *  @param pqueue Coda
 *  @param node Nodo del quale aggiornare la chiave
 *  @param key Nuovo valore della chiave
 */
void priority_queue_increase_key(priority_queue_t * pqueue, priority_queue_node_t * node, priority_queue_key_t key);

/*
 *  Numero di elementi contenuti in una coda a priorità
 */
#define priority_queue_length(pqueue)   (pqueue->length)

/** Verifica se una coda a priorità è vuota */
#define priority_queue_empty(pqueue) (pqueue->length == 0)

/*
 *  Accesso ad un elemento dato l'indice
 */
#define priority_queue_get_element(pqueue, i)  (i >= priority_queue_length(pqueue) ? NULL : pqueue->nodes[i]->value)

/** Calcola l'indice del genitore di un elemento */
#define priority_queue_parent(i)    (i / 2)
/** Calcola l'indice del figlio sinistro di un elemento */
#define priority_queue_left(i)      (2 * i + 1)
/** Calcola l'indice del figlio destro di un elemento */
#define priority_queue_right(i)     (2 * i + 2)

#endif  // std_priority_queue_h
