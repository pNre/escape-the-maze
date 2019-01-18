#ifndef std_queue_h
#define std_queue_h

#include "std/list.h"

/** Coda (un lista con inserimento in coda ed estrazione in testa) */
typedef list_t queue_t;

/**
 *  Creazione di una nuova coda
 *
 *  @return Coda
 */
#define queue_new() list_new(no_functions)

/**
 *  Deallocazione di una coda
 *
 *  @param queue Coda da deallocare
 */
#define queue_delete(queue) list_delete(queue)

/** Controlla se una coda è vuota */
#define queue_empty(queue) (list_length(queue) == 0)

/**
 *  Inserimento di un elemento in una coda
 *
 *  @param queue Coda
 *	@param value Elemento
 */
#define queue_push(queue, value) list_insert(queue, value, INSERT_MODE_TAIL, false, false)

/**
 *  Estrazione di un elemento da una coda
 *
 *  @param queue Coda
 *
 *  @return Elemento estratto
 */
#define queue_pop(queue) list_pop_front(queue)

/**
 *  Testa della coda
 *
 *  @param queue Coda
 *
 *  @return Testa della coda
 */
#define queue_head(queue) list_head(queue)

#endif
