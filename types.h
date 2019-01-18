#ifndef types_h
#define types_h

#include <allegro5/allegro5.h>

#include <string.h>
#include <math.h>

#include "utils.h"

/** Tipo delle funzioni che si occupano di copiare un valore */
typedef void * (* duplicate_function)(void *);
/** Tipo delle funzioni che si occupano di deallocare un valore */
typedef void   (* delete_function)(void *);
/** Tipo delle funzioni che si occupano di confrontare due valori (0 = uguali) */
typedef int    (* compare_function)(void *, void *);
/** Tipo delle funzioni che si occupano di calcolare la dimensione, in bytes, di un valore */
typedef size_t (* size_function)(void *);

/**
 *  Funzioni per gestire un tipo di dato
 */
struct type_functions {

    /** Puntatore alla funzione di copia */
    duplicate_function duplicate;

    /** Puntatore alla funzione di deallocazione */
    delete_function delete;

    /** Puntatore alla funzione di confronto */
    compare_function compare;

    /** Puntatore alla funzione di calcolo della dimensione */
    size_function size;

};

/** 
 *  Tipo di dato booleano 
 */
#ifndef __bool_true_false_are_defined
    #ifndef bool
        #ifdef _Bool
            #define bool _Bool
        #else
            #define bool int
        #endif

        #define true                            1
        #define false                           0
        #define __bool_true_false_are_defined   1
    #endif  // bool
#endif  // __bool_true_false_are_defined

/**
 *  Macro per la dichiarazione delle funzioni necessario per gestire un tipo di dato.
 *  Tutti i parametri sono opzionali ad eccezione di type_name.
 *
 *  @param type_name Nome del tipo di dato
 *  @param delf Funzione di deallocazione
 *  @param szf Funzione per il calcolo della dimensione
 *  @param dupf Funzione di copia
 *  @param compf Funzione di confronto
 */
#define TYPE_FUNCTIONS_DEFINE(...)                                 OVERLOAD(TYPE_FUNCTIONS_DEFINE_, __VA_ARGS__)

#define TYPE_FUNCTIONS_DEFINE_1(type_name)                         struct type_functions type_name ## _ ## functions = {NULL, NULL, NULL, NULL}
#define TYPE_FUNCTIONS_DEFINE_2(type_name, delf)                   struct type_functions type_name ## _ ## functions = {NULL, (delete_function)delf, NULL, NULL}
#define TYPE_FUNCTIONS_DEFINE_3(type_name, delf, szf)              struct type_functions type_name ## _ ## functions = {NULL, (delete_function)delf, NULL, (size_function)szf}
#define TYPE_FUNCTIONS_DEFINE_4(type_name, delf, szf, dupf)        struct type_functions type_name ## _ ## functions = {(duplicate_function)dupf, (delete_function)delf, NULL, (size_function)szf}
#define TYPE_FUNCTIONS_DEFINE_5(type_name, delf, szf, dupf, compf) struct type_functions type_name ## _ ## functions = {(duplicate_function)dupf, (delete_function)delf, (compare_function)compf, (size_function)szf}

#define TYPE_FUNCTIONS_DECLARE(ty)                                  extern struct type_functions ty ## _ ## functions

//  Dimensione minima da allocare quando si crea una stringa
#define STRING_DEFAULT_BUFFER_SIZE 32

//  Funzioni per gestire tipo di dato
TYPE_FUNCTIONS_DECLARE(string);
TYPE_FUNCTIONS_DECLARE(long);
TYPE_FUNCTIONS_DECLARE(float);

TYPE_FUNCTIONS_DECLARE(no);

#define long_value(addr)            (addr ? *((long *)addr) : 0)
#define bool_value(addr)            (!!long_value(addr))
#define float_value(addr)           (addr ? *((float *)addr) : 0.f)

#define long_value_nocheck(addr)    (*((long *)addr))
#define bool_value_nocheck(addr)    (!!long_value_nocheck(addr))
#define float_value_nocheck(addr)   (*((float *)addr))

#endif  // types_h
