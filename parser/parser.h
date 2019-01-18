#ifndef parser_parser_h
#define parser_parser_h

#include <ctype.h>

#include "parser/buffer_parser.h"
#include "parser/file_parser.h"

#include "types.h"

/**
 *  Contiene tutte le informazioni relative ad un parser
 */
struct parser_s {

    /** Sorgente del parsing (ad esempio un file o una stringa) */
    void * buffer;

    /** Destinazione del parsing (hashtable, lista, struct) */
    void * destination;

    /** Offset nel buffer */
    size_t position;

    /** Lunghezza dal buffer */
    size_t length;

    /**
     *  Puntatori alle funzioni utilizzate per gestire il buffer del parser
     */
    struct parser_functions {

        /** Controlla se il parser è arrivato alla fine del buffer */
        int (* is_eos)(struct parser_s *);

        /** Ritorna il carattere alla posizione corrente (parser->position) del buffer */
        int (* current)(struct parser_s *);

        /** Avanza la posizione corrente di un carattere */
        void (* next)(struct parser_s *);

        /** Cambia la posizione corrente */
        void (* seek)(struct parser_s *, long int position);

    } methods;

};

typedef struct parser_s parser_t;

/**
 *  Crea un nuovo parser
 *
 *  @param buffer Buffer sorgente del parsing
 *  @param buffer_length Dimensione del buffer
 *  @param destination Destinazione del parsing
 *  @param functions Funzioni di parsing
 *
 *  @return Parser
 */
parser_t * parser_new(void * buffer, size_t buffer_length, void * destination, struct parser_functions functions);

/**
 *  Dealloca un parser
 *
 *  @param parser Parser da deallocare
 */
void parser_delete(parser_t * parser);

/**
 *  Elimina dal buffer tutte le occorrenze iniziali di un set di caratteri che verificano la condizione:
 *  @code
 *  isspace(chr) || iscntrl(chr)
 *  @endcode
 *
 *  @param parser Parser
 */
void parser_trim(parser_t * parser);

/**
 *  Verifica che il prossimo carattere del buffer sia token
 *
 *  @param parser Parser
 *  @param token
 *
 *  @retval true Trovato
 *  @retval false Non trovato
 */
bool parser_expect(parser_t * parser, int token);

/**
 *  Legge dal buffer un identificativo numerico e lo ritorna come stringa
 *
 *  @param parser Parser
 *
 *  @return Stringa rappresentante un identificativo numerico
 */
char * parser_numeric_identifier(parser_t * parser);

/**
 *  Legge dal buffer un identificativo composto da soli caratteri
 *
 *  @param parser Parser
 *
 *  @return Stringa rappresentante un identificativo
 */
char * parser_alpha_identifier(parser_t * parser);

/**
 *  Legge dal buffer un identificativo alfanumerico
 *
 *  @param parser Parser
 *
 *  @return Stringa rappresentante un identificativo
 */
char * parser_alpha_numeric_identifier(parser_t * parser);

/**
 *  Legge dal buffer un identificativo da caratteri alfanumerici + underscore
 *
 *  @param parser Parser
 *
 *  @return Stringa rappresentante un identificativo
 */
char * parser_alpha_numeric_uscore_identifier(parser_t * parser);

/**
 *  Legge dal buffer un identificativo numerico e lo converte in float
 *
 *  @param parser Parser
 *
 *  @return Valore floating point
 */
float  parser_float(parser_t * parser);

/**
 *  Legge dal buffer un identificativo numerico intero e lo converte in long
 *
 *  @param parser Parser
 *
 *  @return Valore intero
 */
long parser_long_integer(parser_t * parser);

/**
 *  Legge dal buffer una stringa c-style
 *  "prova prova prova"
 *
 *  @param parser Parser
 *
 *  @return Stringa
 */
char * parser_string(parser_t * parser);

#endif  // parser_parser_h
