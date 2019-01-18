#ifndef config_config_h
#define config_config_h

#include "std/hashtable.h"

#include "parser/parser.h"

/**
 *  Tipi di variabili consentite nei file di configurazione
 */
typedef enum {

    CONFIG_VARTYPE_UNKNOWN,

    /**
     *  Intero (long)
     *
     *  @code
     *  int nome_variabile = 1;
     *  @endcode
     */
    CONFIG_VARTYPE_INT,

    /**
     *  Floating point (float)
     *
     *  @code
     *  float nome_variabile = 1.5;
     *  @endcode
     */
    CONFIG_VARTYPE_FLOAT,

    /**
     *  Stringa (char)
     *
     *  @code
     *  string nome_variabile = "ciao";
     *  @endcode
     */
    CONFIG_VARTYPE_STRING,

    /**
     *  Lista (list_t)
     *
     *  @code
     *  list[tipo_elementi] nome_variabile = elemento1, elemento2, ...;
     *  @endcode
     */
    CONFIG_VARTYPE_LIST,

    /**
     *  Dimensione (dimension_t)
     *
     *  @code
     *  size nome_variabile = [w, h];
     *  @endcode
     */
    CONFIG_VARTYPE_SIZE,

    /**
     *  Rettangolo (rectangle_t)
     *
     *  @code
     *  rectangle nome_variabile = [x, y, w, h];
     *  @endcode
     */
    CONFIG_VARTYPE_RECTANGLE,

    /**
     *  Dizionario (hashtable_t)
     *
     *  @code
     *  dictionary nome_variabile = {
     *      tipo_variabile nome_variabile_1 = valore;
     *      tipo_variabile nome_variabile_2 = valore;
     *      ...
     *  }
     *  @endcode
     */
    CONFIG_VARTYPE_DICTIONARY

} config_variable_type;

/**
 *  Legge ed effettua il parsing di un file di configurazione
 *
 *  @param path Percorso del file di configurazione
 *
 *  @return Hashtable contenente le variabili del file di configurazione
 *  @retval NULL in caso di errore
 */
hashtable_t * config_open(char * path);

/**
 *  Converte la rappresentazione come stringa del valore di una variabile nella sua rappresentazione reale
 *
 *  @param parser Parser del file di configurazione
 *  @param type Tipo della variabile
 *  @param contents_type Tipo dei contenuti (nel caso di una lista)
 *  @param *value Valore convertito
 *  @param *functions Funzioni per la gestione del valore
 */
void config_read_value(
    parser_t * parser,
    config_variable_type type,
    config_variable_type contents_type,
    void ** value,
    struct type_functions * functions);

/**
 *  Esegue il parsing della prossima variabile di un file di configurazione
 *
 *  @param parser Parser del file di configurazione
 *
 *  @return Stato del parsing
 *
 *  @retval 0 In caso di parsing effettuato con successo
 *  @retval -1 Tipo non trovato
 *  @retval -2 Tipo non valido
 *  @retval -3 Nome della variabile non valido
 *  @retval -4 = mancante
 *  @retval -5 Valore non valido
 */
int config_parsing(parser_t * parser);

#endif  // config_config_h
