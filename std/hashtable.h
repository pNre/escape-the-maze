#ifndef std_hashtable_h
#define std_hashtable_h

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "types.h"
#include "std/list.h"

#define HASHTABLE_DEFAULT_SIZE 193

/** Tipo dell'hash calcolato per gli elementi della hashtable */
typedef uint32_t hashtable_hash_t;

/**
 *  Hashtable con closed addressing
 */
typedef struct hashtable_s {

    /** Dimensione della tabella */
    size_t size;

    /** Puntatore ad una funzione di hashing */
    hashtable_hash_t (* hash)(void *, size_t);

    /** Tabella contenente le liste di elementi */
    list_t ** table;

    /** Funzioni utilizzate per la gestione delle chiavi */
    struct type_functions key_type;

} hashtable_t;

/**
 *  Nodo di una hashtable
 */
typedef struct hashtable_node_s {

    /** Valore dell'hash della chiave */
    hashtable_hash_t hash;

    /** Chiave per poter identificare l'elemento */
    void * key;

    /** Valore dell'elemento */
    void * value;

    /** Tipo dell'elemento */
    struct type_functions value_type;

    /** Se deallocare o meno il valore dell'elemento quando si dealloca il nodo */
    bool free;

    /** Tabella di appartenenza */
    hashtable_t * table;

} hashtable_node_t;

TYPE_FUNCTIONS_DECLARE(hashtable);
TYPE_FUNCTIONS_DECLARE(hashtable_node);

/**
 *  Creazione di una nuova hashtable
 *
 *  @param size Dimensione della tabella
 *  @param key_type Tipo della chiave degli elementi
 *
 *  @return Hashtable
 */
hashtable_t * hashtable_new(size_t size, struct type_functions key_type);

/**
 *  Deallocazione di una hashtable
 *
 *  @param hashtable Hashtable da deallocare
 */
void hashtable_delete(hashtable_t * hashtable);

/**
 *  Crea una copia di una hashtable
 *
 *  @param hashtable Hashtable da copiare
 *
 *	@return Copia
 */
hashtable_t * hashtable_duplicate(hashtable_t * table);

/**
 *  Inserimento di un nuovo elemento in una hashtable
 *
 *  @param table Tabella di destinazione
 *  @param key Chiave
 *  @param value Valore dell'elemento
 *  @param value_type Tipo dell'elemento
 *  @param copy Se copiare o meno il valore dell'elemento prima dell'inserimento
 *  @param free Se deallocare o meno l'elemento in fase di cancellazione
 */
#define hashtable_insert(...)                 OVERLOAD(hashtable_insert_, __VA_ARGS__)

#define hashtable_insert_3(table, key, value)                           hashtable_insert_element(table, key, value, no_functions, false, false)
#define hashtable_insert_4(table, key, value, value_type)               hashtable_insert_element(table, key, value, value_type, false, false)
#define hashtable_insert_5(table, key, value, value_type, copy)         hashtable_insert_element(table, key, value, value_type, copy, copy)
#define hashtable_insert_6(table, key, value, value_type, copy, free)   hashtable_insert_element(table, key, value, value_type, copy, free)

void hashtable_insert_element(hashtable_t * table, void * key, void * value, struct type_functions value_type, int copy, int free);

/**
 *  Sostituzione di un nuovo elemento esistente in una hashtable
 *
 *  @param table Tabella di destinazione
 *  @param key Chiave
 *  @param value Valore dell'elemento
 *  @param copy Se copiare o meno il valore dell'elemento prima dell'inserimento
 *
 *  @return Esito della sostituzione
 */
#define hashtable_replace(...)                OVERLOAD(hashtable_replace_, __VA_ARGS__)

#define hashtable_replace_3(table, key, value)             hashtable_replace_element(table, key, value, 0)
#define hashtable_replace_4(table, key, value, copy)       hashtable_replace_element(table, key, value, copy)

bool hashtable_replace_element(hashtable_t * table, void * key, void * value, int copy);

/**
 *  Rimuove un elemento da una hashtable
 *
 *  @param table Tabella
 *  @param key Chiave dell'elemento da rimuovere
 */
#define hashtable_remove(table, key)    \
    hashtable_remove_element(table, key)

void hashtable_remove_element(hashtable_t * table, void * key);

/**
 *  Cerca un elemento in una hashtable
 *
 *  @param table Tabella
 *  @param key Chiave dell'elemento da cercare
 *
 *  @return Valore dell'elemento
 *  @retval NULL Se l'elemento non viene trovato
 */
#define hashtable_search(...)                 OVERLOAD(hashtable_search_, __VA_ARGS__)

#define hashtable_search_2(table, key)    \
    hashtable_search_element(table, key)

#define hashtable_search_3(table, key, type)    \
    type ## _value_nocheck(hashtable_search_element(table, key))

void * hashtable_search_element(hashtable_t * table, void * key);

/**
 *  Interazione su ogni coppia chiave - valore contenuta nella tabella
 *
 *  @param table Tabella
 *  @param data Dati extra da passare al callback
 *  @param iteration_function Callback
 */
void hashtable_iterate(hashtable_t * table, void * data, void (* iteration_function)(hashtable_t * table, void * data, void * key, void * value, struct type_functions value_type));

/**
 *  Cerca una lista di elementi assegnata all'hash di una certa chiave
 *
 *  @param table Tabella
 *  @param key Chiave dell'elemento da cercare
 *
 *  @return Lista
 *  @retval NULL Se non c'è nessuna lista che corrisponde all'hash della chiave
 */
list_t * hashtable_chain(hashtable_t * table, void * key);

/**
 *  Funzione di hashing utilizzata di default
 *  http://www.isthe.com/chongo/tech/comp/fnv/
 *
 *  @param input Buffer del quale calcolare l'hash
 *  @param length Lunghezza del buffer in bytes
 *
 *  @return Hash
 */
hashtable_hash_t fnv1(void * input, size_t length);

#endif  // std_hashtable_h
