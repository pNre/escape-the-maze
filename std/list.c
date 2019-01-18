#include <stdio.h>
#include <stdarg.h>

#include "utils.h"

#include "misc/random.h"

#include "std/list.h"

/**
 *  Crea un nuovo nodo per l'inserimento in una lista
 *
 *  @param list Lista
 *  @param value Valore del nodo
 *  @param copy Se copiare o meno il valore del nodo
 *
 *  @return Nodo
 */
list_node_t * list_node_new(list_t * list, void * value, int copy, int free)
{

    list_node_t * node = memalloc(list_node_t);

    //  copia o meno il valore passato, nel caso che, ad esempio,
    //  fosse necessario liberare la memoria dopo la chiamata la funzione
    node->value = copy && list->element_type.duplicate ? list->element_type.duplicate(value) : value;

    //  se il valore è stato copiato poi va deallocato
    node->free = (copy || free);

    node->next = NULL;
    node->prev = NULL;

    return node;

}

/**
 *  Dealloca un nodo
 *
 *  @param list Lista
 *  @param node Nodo
 */
void list_node_delete(list_t * list, list_node_t * node)
{

    if (!node)
        return;

    //  1: se il valore va deallocato, si dealloca
    if (node->free && list->element_type.delete)
        list->element_type.delete(node->value);

    //  2: deallocazione struttura nodo
    memfree(node);

}

list_t * list_new(struct type_functions type)
{

    list_t * list = memalloc(list_t);

    list->head = NULL;
    list->tail = NULL;

    list->element_type = type;

    list->length = 0;

    return list;

}

void list_delete(list_t * list)
{

    if (!list)
        return;

    //  1: deallocazione nodi
    list_node_t * node = list->head;

    while (node) {
        list_node_t * next = node->next;
        list_node_delete(list, node);
        node = next;
    }

    //  2: deallocazione lista
    memfree(list);

}

void list_insert_element(list_t * list, void * element, int insert_mode, int copy, int free)
{

    if (!list || !element)
        return;

    //  creazione del nodo da inserire
    list_node_t * node = list_node_new(list, element, copy, free);

    //  inserimento in lista vuota
    if (!list->length) {

        list->head = list->tail = node;

    }
    //  inserimento in testa
    else if (insert_mode == INSERT_MODE_HEAD) {

        node->next = list->head;
        list->head = node;

    }
    //  inserimento in coda
    else {

        node->prev = list->tail;
        node->prev->next = node;
        list->tail = node;

    }

    list->length++;

}

/**
 *  Elimina un nodo da una lista
 *
 *  @param list Lista
 *  @param node Nodo
 */
void list_remove_node(list_t * list, list_node_t * node)
{

    if (!list || !node)
        return;

    //  eliminazione testa
    if (!node->prev) {

        list->head = node->next;

        if (list->head)
            list->head->prev = NULL;

    }
    //  eliminazione coda
    else if (!node->next) {

        list->tail = node->prev;

        if (list->tail)
            list->tail->next = NULL;

    } else {
        //  eliminazione di un nodo qualsiasi
        node->prev->next = node->next;
        if (node->next)
            node->next->prev = node->prev;
    }

    list_node_delete(list, node);
    list->length--;

    //  la lista si è svuotata
    if (!list->length)
        list->head = list->tail = NULL;

}

void list_remove_element(list_t * list, void * element)
{

    if (!list || !element)
        return;

    if (!list->length)
        return;

    list_node_t * node = list->head;

    while (node) {

        //  elemento trovato
        if ((list->element_type.compare && !list->element_type.compare(node->value, element)) || element == node->value) {

            list_remove_node(list, node);
            return;

        }

        node = node->next;

    }

}

/**
 *  Estra il nodo ad un certo indice della lista
 *
 *  @param list Lista
 *  @param index Indice del nodo
 */
list_node_t * list_node_at_index(list_t * list, int index)
{

    if (!list->length)
        return NULL;

    int current = 0;
    list_node_t * node = list->head;

    while (node) {

        //  elemento trovato
        if (current == index)
            return node;

        node = node->next;
        current++;

    }

    return NULL;

}

void * list_pop_random(list_t * list)
{

    list_node_t * node = list_node_at_index(list, random_int(0, list->length - 1));

    if (!node)
        return NULL;

    //  valore contenuto nel nodo
    void * value = node->value;

    //  si elimina il nodo
    list_remove_node(list, node);

    return value;

}

void * list_pop_front(list_t * list)
{

    list_node_t * head = list->head;

    if (!head)
        return NULL;

    //  valore contenuto nel nodo
    void * value = head->value;

    //  si elimina il nodo
    list_remove_node(list, head);

    return value;

}

void * list_get_random(list_t * list)
{

    list_node_t * node = list_node_at_index(list, random_int(0, list->length - 1));

    if (!node)
        return NULL;

    //  valore contenuto nel nodo
    return node->value;

}

TYPE_FUNCTIONS_DEFINE(list, list_delete);
