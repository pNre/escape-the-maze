#include <stdio.h>

#include "utils.h"

#include "parser/parser.h"

void parser_trim(parser_t * parser)
{

    int chr;

    while ((chr = parser->methods.current(parser)) && (isspace(chr) || iscntrl(chr))) {

        parser->methods.next(parser);
    }

    if (chr == EOF)
        parser->methods.next(parser);

}

/**
 *  Controlla che un carattere sia alfanumerico o un underscore
 *
 *  @param chr Carattere
 */
sinline int isalnum_uscore(int chr)
{
    return chr == '_' || isalnum(chr);
}

/**
 *  Controlla che un carattere possa far parte della rappresentazione di un float
 *
 *  @param c Carattere
 */
sinline int isfloat(int c)
{
    return isdigit(c) || c == '.';
}

/**
 *  Ritorna un identificativo nel quale ogni carattere verifica una certa condizione
 *
 *  @param parser Parser
 *  @param check Puntatore alla funzione che rappresenta la condizione che ogni carattere deve verificare
 *
 *  @return Identificativo
 */
char * parser_identifier(parser_t * parser, int (* check)(int))
{

    char * id = NULL;
    unsigned int id_length = 0;

    int chr;

    parser_trim(parser);

    //  il loop continua finchè ci sono caratteri da leggere e
    //  il carattere appena letto è valido
    while ((chr = parser->methods.current(parser)) && check(chr)) {

        //  se la lunghezza raggiunge quella del buffer, si raddoppia il buffer
        if (!(id_length % STRING_DEFAULT_BUFFER_SIZE)) {
            id = memrealloc(id, char, id_length + STRING_DEFAULT_BUFFER_SIZE + 1);
        }

        id[id_length++] = chr;

        //  carattere successivo
        parser->methods.next(parser);

    }

    if (id)
        id[id_length] = 0;

    return id;

}

char * parser_numeric_identifier(parser_t * parser)
{

    return parser_identifier(parser, isdigit);

}

char * parser_alpha_numeric_identifier(parser_t * parser)
{

    return parser_identifier(parser, isalnum);

}

char * parser_alpha_numeric_uscore_identifier(parser_t * parser)
{

    return parser_identifier(parser, isalnum_uscore);

}

char * parser_alpha_identifier(parser_t * parser)
{

    return parser_identifier(parser, isalpha);

}

float parser_float(parser_t * parser)
{

    parser_trim(parser);

    //  controlla la presenza del segno per non avere problemi
    //  nel parsing dell'id numerico
    int sign = parser->methods.current(parser);

    if (sign == '+' || sign == '-')
        parser->methods.next(parser);
    else
        sign = 0;

    char * number = parser_identifier(parser, isfloat);

    //  se ci sono problemi con il parsing ritorna un valore di default 0
    if (!number)
        return 0;

    //  char * -> float
    float flt = strtof(number, NULL);

    memfree(number);

    //  se c'è un - allora è negativo
    if (sign == '-')
        flt *= -1.;

    return flt;

}

long parser_long_integer(parser_t * parser)
{

    parser_trim(parser);

    //  controlla la presenza del segno per non avere problemi
    //  nel parsing dell'id numerico
    int sign = parser->methods.current(parser);

    if (sign == '+' || sign == '-')
        parser->methods.next(parser);
    else
        sign = 0;

    char * number = parser_numeric_identifier(parser);

    //  se ci sono problemi con il parsing ritorna un valore di default 0
    if (!number)
        return 0;

    //  char * -> long
    long lval = strtol(number, NULL, 10);

    memfree(number);

    //  se c'è un - allora è negativo
    if (sign == '-')
        lval *= -1;

    return lval;

}

char * parser_string(parser_t * parser)
{

    char * str = NULL;
    unsigned int str_length = 0;

    int chr;

    parser_trim(parser);

    //  non comincia con gli apici doppi
    if (parser->methods.current(parser) != '"')
        return NULL;

    parser->methods.next(parser);

    //  finchè c'è qualcosa da leggere e non si incontra un apice
    while ((chr = parser->methods.current(parser)) && chr != '"') {

        if (!(str_length % STRING_DEFAULT_BUFFER_SIZE)) {
            str = memrealloc(str, char, str_length + STRING_DEFAULT_BUFFER_SIZE + 1);
        }

        str[str_length++] = chr;

        parser->methods.next(parser);

    }

    //  se è stato letto almeno un carattere si inserisce il terminatore di stringa
    if (str)
        str[str_length] = 0;

    if (chr == '"')
        parser->methods.next(parser);

    return str;

}

bool parser_expect(parser_t * parser, int token)
{

    parser_trim(parser);

    bool r = (parser->methods.current(parser) == token);

    if (r)
        parser->methods.next(parser);

    return r;

}

parser_t * parser_new(void * buffer, size_t buffer_length, void * destination, struct parser_functions functions)
{

    parser_t * parser = memalloc(parser_t);

    parser->buffer = buffer;
    parser->destination = destination;

    parser->methods = functions;

    parser->position = 0;
    parser->length = buffer_length;

    return parser;

}

void parser_delete(parser_t * parser)
{

    parser->destination = NULL;

    memfree(parser);

}
