#ifndef std_list_h
#define std_list_h

#include <limits.h>

#include "types.h"

/**
 *  Modalità di inserimento nella lista
 */
enum {

    /** Inserimento in testa */
    INSERT_MODE_HEAD,

    /** Inserimento in coda */
    INSERT_MODE_TAIL

};

/**
 *  Nodo di una lista
 */
typedef struct list_node_s {

    /** Valore del nodo */
    void * value;

    /** Collegamento al nodo successivo nella lista */
    struct list_node_s * next;

    /** Collegamento al nodo precedente nella lista */
    struct list_node_s * prev;

    /** Se deallocare o meno il valore del nodo quando si dealloca il nodo */
    bool free;

} list_node_t;

/**
 *  Lista
 */
typedef struct list_s {

    /** Nodo in testa alla lista */
    list_node_t * head;

    /** Nodo in coda alla lista */
    list_node_t * tail;

    /** Funzioni per la gestione dei valori dei nodi */
    struct type_functions element_type;

    /** Numero di elementi nella lista */
    size_t length;

} list_t;

TYPE_FUNCTIONS_DECLARE(list);

/**
 *  Creazione di una nuova lista
 *
 *  @param type_funcs Funzione per gestire il tipo degli elementi della lista
 *
 *  @return Lista
 */
list_t * list_new(struct type_functions);

/**
 *  Deallocazione di una lista
 *
 *  @param list Lista da deallocare
 */
void list_delete(list_t * list);

/** Lunghezza (numero di elementi) di una lista */
#define list_length(list)   \
    (list ? list->length : 0)

/** Valore del nodo in testa ad una lista */
#define list_head(list) \
    ((list && list->head) ? list->head->value : NULL)

/** Valore del nodo in coda ad una lista */
#define list_tail(list) \
    ((list && list->tail) ? list->tail->value : NULL)

/** Controlla se una lista è vuota */
#define list_empty(list)  (list_length(list) == 0)

/**
 *  Inserimento di un elemento in una lista
 *
 *  @param list Lista
 *  @param element Valore dell'elemento
 *  @param mode Modalità di inserimento
 *  @param copy Se copiare o meno il valore prima di inserirlo
 */
#define list_insert(...)                                OVERLOAD(list_insert_, __VA_ARGS__)

#define list_insert_2(list, element)                    list_insert_element(list, element, INSERT_MODE_TAIL, true, true)
#define list_insert_3(list, element, mode)              list_insert_element(list, element, mode, true, true)
#define list_insert_4(list, element, mode, copy)        list_insert_element(list, element, mode, copy, copy)
#define list_insert_5(list, element, mode, copy, free)  list_insert_element(list, element, mode, copy, free)

void list_insert_element(list_t * list, void * element, int mode, int copy, int free);

/**
 *  Cerca e rimuove un elemento da una lista
 *
 *  @param list Lista
 *  @param element Valore dell'elemento
 */
#define list_remove(list, element)  \
    list_remove_element(list, element);

void list_remove_element(list_t * list, void * element);

/**
 *  Estrae l'elemento in testa alla lista
 *
 *  @param list Lista
 *
 *  @return Valore dell'elemento
 *  @retval NULL Se la lista è vuota
 */
void * list_pop_front(list_t * list);

/**
 *  Estrae un elemento casuale dalla lista
 *
 *  @param list Lista
 *
 *  @return Valore dell'elemento
 *  @retval NULL Se la lista è vuota
 */
void * list_pop_random(list_t * list);

/**
 *  Ritorna il valore di un elemento casuale della lista
 *
 *  @param list Lista
 *
 *  @return Valore dell'elemento
 *  @retval NULL Se la lista è vuota
 */
void * list_get_random(list_t * list);

/**
 *  Foreach su una variabile di tipo lista
 *
 *  @code
 *  foreach(list, char *, string) {
 *      printf("%s\n", string);
 *  }
 *  @endcode
 *
 *  @param list Lista
 *  @param value_type Tipo del valore contenuto nei nodi
 *  @param value_var_name Nome da assegnare alla variabile contenente il valore
 */
#define foreach(list, value_type, value_var_name) \
    list_node_t * __node_##value_var_name;  \
    value_type value_var_name;              \
    if (list)   \
        for (   \
            __node_##value_var_name = list->head, value_var_name = __node_##value_var_name ? __node_##value_var_name->value : NULL;   \
            __node_##value_var_name != NULL;  \
            __node_##value_var_name = __node_##value_var_name ? __node_##value_var_name->next : NULL, value_var_name = __node_##value_var_name ? __node_##value_var_name->value : NULL)

#endif
