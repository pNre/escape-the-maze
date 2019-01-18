#include <stdio.h>

#include "parser/file_parser.h"

int file_eos(parser_t * parser)
{

    FILE * f = (FILE *)parser->buffer;
    
    return feof(f) || parser->methods.current(parser) == EOF;

}

int file_current(parser_t * parser)
{
    
    FILE * f = (FILE *)parser->buffer;
    
    int chr = fgetc(f);

    if (chr == EOF) {
        return chr;
    }

    fseek(f, parser->position, SEEK_SET);
    
    return chr;

}

void file_next(parser_t * parser)
{

    FILE * f = (FILE *)parser->buffer;

    if (!fseek(f, 1L, SEEK_CUR))
        parser->position = ftell(f);

}

void file_seek(parser_t * parser, long int position)
{
    
    FILE * f = (FILE *)parser->buffer;
    
    fseek(f, position - 1, SEEK_SET);

    parser->position = ftell(f);

}

struct parser_functions file_parser_functions = {

    file_eos,
    file_current,
    file_next,
    file_seek

};
