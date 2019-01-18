#include <stdio.h>

#include "types.h"

/**
 *  Dealloca un'area di memoria precedentemente allocata
 *
 *  @param mem Memoria
 */
void delete_any(void * mem)
{
    memfree(mem);
}

/**
 *  Copia una stringa
 *
 *  @param string Stringa
 *
 *  @return Copia
 */
void * duplicate_string(void * string)
{
    return strdup(string);
}

/**
 *  Confronta due stringhe
 *
 *  @param string_a Stringa A
 *  @param string_b Stringa B
 *
 *  @return Risultato del confronto
 */
int compare_strings(void * string_a, void * string_b)
{
    return strcmp(string_a, string_b);
}

/**
 *  Calcola la dimensione, in bytes, di una stringa
 *
 *  @param string Stringa
 *
 *  @return Dimensione
 */
size_t sizeof_string(void * string)
{
    return strlen(string);
}

/**
 *  Copia un long
 *
 *  @param lval Intero da copiare
 *
 *  @return Copia
 */
void * duplicate_long(void * lval)
{
    long * ldup = memalloc(long);
    *ldup = long_value(lval);
    return ldup;
}

/**
 *  Confronta due long
 *
 *  @param l_a Intero A
 *  @param l_b Intero B
 *
 *  @return Risultato del confronto
 */
int compare_longs(void * l_a, void * l_b)
{
    return (int)(long_value(l_a) - long_value(l_b));
}

/**
 *  Calcola la dimensione, in bytes, di un long
 *
 *  @param lval Inutilizzato
 *
 *  @return Dimensione
 */
size_t sizeof_long(void * lval)
{
    return sizeof(long);
}

/**
 *  Copia un float
 *
 *  @param flt Float da copiare
 *
 *  @return Copia
 */
void * duplicate_float(void * flt)
{
    float * flt_dup = memalloc(float);
    *flt_dup = float_value(flt);
    return flt_dup;
}

/**
 *  Confronta due floats
 *
 *  @param flt_a Float A
 *  @param flt_a Float B
 *
 *  @return Risultato del confronto
 */
int compare_floats(void * flt_a, void * flt_b)
{

    float a = fabsf(float_value(flt_a));
    float b = fabsf(float_value(flt_b));

    float c = fmax(a, b);

    return ((c == 0.0 ? 0.0 : fabsf(a - b) / c)) <= __FLT_EPSILON__;

}

/**
 *  Calcola la dimensione, in bytes, di un float
 *
 *  @param flt Inutilizzato
 *
 *  @return Dimensione
 */
size_t sizeof_float(void * flt)
{
    return sizeof(float);
}

TYPE_FUNCTIONS_DEFINE(no);

TYPE_FUNCTIONS_DEFINE(string, delete_any, sizeof_string, duplicate_string, compare_strings);
TYPE_FUNCTIONS_DEFINE(long, delete_any, sizeof_long, duplicate_long, compare_longs);
TYPE_FUNCTIONS_DEFINE(float, delete_any, sizeof_float, duplicate_float, compare_floats);

