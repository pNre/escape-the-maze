#ifndef utils_h
#define utils_h

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "preprocessor.h"
#include "types.h"

#ifdef _MSC_VER
    #define sinline static __inline       /* use __inline (VC++ specific) */
#else
    #define sinline static inline         /* use standard inline */
#endif

/**
 *  Numero di elementi in un array statico
 *
 *  @param array Array
 */
#define array_count(array) (sizeof(array) / sizeof(array[0]))

/**
 *  Logging degli errori sullo standard error
 *
 *  @param fmt Formato della stringa
 *  @param ... Parametri
 */
#define errorf(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)

/**
 *  Logging dei messaggi di debug su standard output
 *
 *  @param fmt Formato della stringa
 *  @param ... Parametri
 */
#define debugf(fmt, ...) printf(fmt, __VA_ARGS__)

/**
 *  Malloc con controllo del valore di ritorno.
 *  In caso di fallimento dell'allocazione il programma viene terminato.
 *
 *  @param size Dimensione della memoria da allocare
 *  @param clear Se azzerare la memoria allocata
 *
 *  @return Memoria allocata
 */
sinline void * safe_malloc(size_t size, bool clear)
{

    void * ptr = malloc(size);

    if (!ptr) {
        errorf("Impossibile allocare %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }

    if (clear)
        memset(ptr, 0, size);

    return ptr;

}

/**
 *  Realloc con controllo del valore di ritorno.
 *  In caso di fallimento dell'allocazione il programma viene terminato.
 *
 *  @param ptr Puntatore all'area di memoria da ri-allocare
 *  @param size Dimensione della nuova area di memoria da allocare
 *
 *  @return Memoria allocata
 */
sinline void * safe_realloc(void * ptr, size_t size)
{

    void * reptr = realloc(ptr, size);

    if (!reptr) {
        errorf("Impossibile ri-allocare %zu bytes\n", size);
        exit(EXIT_FAILURE);
    }

    return reptr;

}

/**
 *  Allocazione di un'area di memoria.
 *  type è l'unico parametro obbligatorio
 *
 *  @param type Tipo del dato da allocare
 *  @param count Numero di volte che il tipo va allocato (dimensione memoria allocata = sizeof(type) * count)
 *  @param clear Se azzerare l'area di memoria allocata
 *
 *  @return Puntatore all'area di memoria allocata
 */
#define memalloc(...)                   OVERLOAD(memalloc_, __VA_ARGS__)

#define memalloc_1(type)                safe_malloc(sizeof(type), 0)
#define memalloc_2(type, count)         safe_malloc((count * sizeof(type)), 0)
#define memalloc_3(type, count, clear)  safe_malloc((count * sizeof(type)), clear)

/**
 *  Ri-allocazione di un'area di memoria.
 *  count è un parametro opzionale
 *
 *  @param addr Area di memoria da ri-allocare
 *  @param type Tipo del dato da allocare
 *  @param count Numero di volte che il tipo va allocato (dimensione memoria allocata = sizeof(type) * count)
 *
 *  @return Puntatore all'area di memoria ri-allocata
 */
#define memrealloc(...)                  OVERLOAD(memrealloc_, __VA_ARGS__)

#define memrealloc_2(addr, type)         safe_realloc(addr, sizeof(type))
#define memrealloc_3(addr, type, count)  safe_realloc(addr, sizeof(type) * (count))

/**
 *  De-allocazione di un'area di memoria.
 *
 *  @param mem Area di memoria da de-allocare
 */
#define memfree(mem)                    free(mem)

#endif  // utils_h
