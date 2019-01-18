#include <stdio.h>

#include "parser/parser.h"
#include "parser/buffer_parser.h"

int buffer_eos(parser_t * parser)
{

    return (parser->position >= parser->length);

}

int buffer_current(parser_t * parser)
{

    if (parser->position >= parser->length)
        return EOF;

    return ((char *)parser->buffer)[parser->position];

}

void buffer_next(parser_t * parser)
{

    parser->position++;

}

void buffer_seek(parser_t * parser, long int off)
{

    parser->position = off;

}

struct parser_functions buffer_parser_functions = {

    buffer_eos,
    buffer_current,
    buffer_next,
    buffer_seek

};
