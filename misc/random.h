#ifndef misc_random_h
#define misc_random_h

#include "utils.h"
#include "types.h"

#include "misc/random.h"

/**
 *  Inizializza il generatore di numeri causali, Well Equidistributed Long period Linear Random Number Generator (WELL)
 */
void well512_initialize(void);

/**
 *  Genera un numero pseudo-casuale
 */
unsigned long well512_random(void);

/**
 *  Calcola un float random x
 *
 *  @return Float random calcolato
 */
#define random_float()  (well512_random() / (float)ULONG_MAX)

/**
 *  Calcola un intero random x, tale che min <= x <= max
 *
 *  @param min Limite inferiore
 *  @param max Limite superiore
 *
 *  @return Intero random calcolato
 */
#define random_int(min, max)  ((unsigned int)(min + random_float() * (max - min + 1)))

/**
 *  Calcola un intero random x, in un certo intervallo
 *
 *  @param r Intervallo
 *
 *  @return Intero random calcolato
 */
#define random_int_in_range(r)  ((int)((int)r.min + well512_random() * ((int)r.max - (int)r.min + 1)))

/**
 *  Ritorna vero con una certa probabilità
 *
 *  @param prob Probabilità che la funzione ritorni vero (1. = 100%, 0. = 0%)
 *
 *  @return true/false
 */
#define random_bool(prob)     (well512_random() < (float)prob * ((float)ULONG_MAX + 1.))

/**
 *  Genera una stringa composta da caratteri casuali
 *
 *  @param max_length Lunghezza massima della stringa
 */
char * random_string(size_t max_length);

#endif
