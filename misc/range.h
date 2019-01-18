#ifndef misc_range_h
#define misc_range_h

#include <math.h>

/** Rappresentazione di un intervallo tra 2 valori */
typedef struct {

    /** Valore minimo dell'intervallo */
    float min;

    /** Valore massimo dell'intervallo */
    float max;

} range_t;

/**
 *  Creazione di un intervallo
 *
 *  @param min Limite inferiore
 *  @param max Limite superiore
 */
#define RangeMake(min, max)             ((range_t){min, max})

/**
 *  Calcolo del centro di un intervallo
 *
 *  @param r Intervallo
 */
#define RangeCenter(r)                  ((r.min + r.max) * 2.)

/**
 *  Calcolo della dimensione di un intervallo
 *
 *  @param r Intervallo
 */
#define RangeSize(r)                    (r.max - r.min)

/**
 *  Creazione di un intervallo di ampiezza 2 * length a partire dal
 *  centro di un intervallo
 *
 *  @param r Intervallo
 *  @param length Semiampiezza
 */
#define RangeFromRangeCenter(r, length) RangeMake(RangeCenter(r) - length, RangeCenter(r) + length)

/**
 *  Verifica se un valore è interno ad un intervallo
 *
 *  @param r Intervallo
 *  @param value Value
 */
#define RangeContainsValue(r, value)    (r.min <= value && value <= r.max)

/**
 *  Si assicura che un valore sia interno ad un intervallo
 *
 *  @param r Intervallo
 *  @param value Value
 */
#define RangeNormalizedValue(r, value)   fmaxf(r.min, fminf(r.max, value))

#endif
