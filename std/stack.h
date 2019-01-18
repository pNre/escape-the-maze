#ifndef std_stack_h
#define std_stack_h

#include "std/list.h"
#include "std/queue.h"

/** Stack (un lista con inserimento ed estrazione in testa) */
typedef list_t sstack_t;

/**
 *  Creazione di un nuovo stack
 *
 *  @return Stack
 */
#define stack_new() list_new(no_functions)

/**
 *  Deallocazione di uno stack
 *
 *  @param stack Stack da deallocare
 */
#define stack_delete(stack) list_delete(stack)

/** Valore del nodo in testa ad uno stack */
#define stack_head(stack) list_head(stack)

/** Controlla se uno stack è vuoto */
#define stack_empty(stack) (list_length(stack) == 0)

/**
 *  Inserimento di un elemento in uno stack
 *
 *  @param stack Stack
 *	@param value Elemento
 */
#define stack_push(stack, value) list_insert(stack, value, INSERT_MODE_HEAD, 0)

/**
 *  Estrazione di un elemento da uno stack
 *
 *  @param stack Stack
 *
 *  @return Elemento estratto
 */
#define stack_pop(stack) list_pop_front(stack)

#endif
