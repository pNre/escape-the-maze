#include <stdio.h>
#include <string.h>

#include "std/hashtable.h"
#include "std/list.h"

hashtable_hash_t fnv1(void * input, size_t length)
{

    char * data = (char *)input;
    unsigned int offset = 0;

    const hashtable_hash_t FNV_prime = 0x01000193;
    const hashtable_hash_t offset_basis = 0x811c9dc5;

    hashtable_hash_t hash = offset_basis;

    for (; offset < length; offset++) {
        hash ^= data[offset];
        hash *= FNV_prime;
    }

    return hash;

}

hashtable_node_t * hashtable_node_new(hashtable_t * table, void * key, void * value, struct type_functions value_type, int copy, int free)
{

    if (!table || !key || !value)
        return NULL;

    hashtable_node_t * node = memalloc(hashtable_node_t);

    //  se copy = 1 ed è definita una funzione di copia, è copiato il valore
    node->value = copy && value_type.duplicate ? value_type.duplicate(value) : value;

    //  funzioni sul valore
    node->value_type = value_type;

    //  se era da copiare, va poi deallocato
    node->free = (copy || free);

    //  se la chiave non ha associata una funzione di copia, si salva il riferimento
    node->key = table->key_type.duplicate ? table->key_type.duplicate(key) : key;

    //  dimensione della chiave
    size_t key_size = table->key_type.size ? table->key_type.size(key) : 0;

    //  hash della chiave
    node->hash = table->hash(key, key_size);

    node->table = table;

    return node;

}

void hashtable_node_delete(hashtable_node_t * node)
{

    if (!node)
        return;

    //  1: rimozione contenuto e chiave, se necessario
    if (node->free && node->value_type.delete) {
        node->value_type.delete(node->value);
    }

    node->table->key_type.delete(node->key);

    //  2: eliminazione nodo
    memfree(node);

}

list_t * hashtable_chain(hashtable_t * table, void * key)
{

    if (!table || !key)
        return NULL;

    //  calco hash della chiave
    hashtable_hash_t hash = table->hash(key, table->key_type.size(key));

    //  individuazione locazione dell'array
    return table->table[(hash % table->size)];

}

void hashtable_remove_element(hashtable_t * table, void * key)
{

    //  individuazione locazione dell'array
    list_t * list = hashtable_chain(table, key);

    if (!list)
        return;

    //  ricerca dell'elemento giusto nella catena
    list_node_t * current = list->head;

    while (current) {

        //  nodo della tabella
        hashtable_node_t * node = current->value;

        //  trovato
        if (!table->key_type.compare(node->key, key)) {

            list_remove_element(list, current);
            break;

        }

        current = current->next;

    }

}

void hashtable_insert_element(hashtable_t * table, void * key, void * value, struct type_functions value_type, int copy, int free)
{

    //  nuovo nodo
    hashtable_node_t * node = hashtable_node_new(table, key, value, value_type, copy, free);

    if (!node)
        return;
    
    //  individuazione locazione dell'array
    int loc = (node->hash % table->size);
    list_t * list = table->table[loc];

    //  se è vuota si crea la lista
    if (!list)
        table->table[loc] = list = list_new(hashtable_node_functions);

    //  inserimento
    list_insert(list, node);

}

bool hashtable_replace_element(hashtable_t * table, void * key, void * value, int copy)
{

    //  individuazione locazione dell'array
    list_t * list = hashtable_chain(table, key);

    if (!list)
        return false;

    //  ricerca dell'elemento giusto nella catena
    foreach(list, hashtable_node_t *, node) {

        //  trovato
        if (!table->key_type.compare(node->key, key)) {

            if (node->free && node->value_type.delete)
                node->value_type.delete(node->value);

            if (copy && node->value_type.duplicate)
                node->value = node->value_type.duplicate(value);
            else
                node->value = value;

            node->free = copy;

            return true;

        }

    }

    return false;

}

void * hashtable_search_element(hashtable_t * table, void * key)
{

    //  individuazione locazione dell'array
    list_t * list = hashtable_chain(table, key);

    if (!list)
        return NULL;

    //  ricerca dell'elemento giusto nella catena
    foreach(list, hashtable_node_t *, node) {

        //  trovato
        if (!table->key_type.compare(node->key, key)) {

            return node->value;

        }

    }

    return NULL;

}

void hashtable_iterate(hashtable_t * table, void * data, void (* iteration_function)(hashtable_t * table, void * data, void * key, void * value, struct type_functions value_type))
{

    unsigned int i;
    for (i = 0; i < table->size; i++) {

        if (!table->table[i])
            continue;

        foreach(table->table[i], hashtable_node_t *, node) {

            iteration_function(table, data, node->key, node->value, node->value_type);

        }

    }

}

hashtable_t * hashtable_new(size_t table_size, struct type_functions key_type)
{

    if (!table_size)
        table_size = HASHTABLE_DEFAULT_SIZE;

    //  allocazione hashtable e array delle liste di elementi
    hashtable_t * table = memalloc(hashtable_t);

    table->size = table_size;
    table->table = memalloc(list_t *, table_size, 1);

    table->key_type = key_type;

    //  funzione di default per l'hashing
    table->hash = fnv1;

    return table;

}

void hashtable_delete(hashtable_t * table)
{

    if (!table)
        return;

    //  1: deallocazione elementi contenuti
    size_t i;
    for (i = 0; i < table->size; i++)
        list_delete(table->table[i]);

    //  2: deallocazione array
    memfree(table->table);

    //  3: deallocazione struttura
    memfree(table);

}

hashtable_t * hashtable_duplicate(hashtable_t * table)
{

    hashtable_t * duplicate = hashtable_new(table->size, table->key_type);

    unsigned int i;
    for (i = 0; i < table->size; i++) {

        if (!table->table[i])
            continue;

        foreach(table->table[i], hashtable_node_t *, node) {

            hashtable_insert(duplicate, node->key, node->value, node->value_type, node->free);

        }

    }

    return duplicate;

}

TYPE_FUNCTIONS_DEFINE(hashtable_node, hashtable_node_delete);
TYPE_FUNCTIONS_DEFINE(hashtable, hashtable_delete, NULL, hashtable_duplicate);
