#include <stdio.h>

#include "config/config.h"

#include "parser/parser.h"
#include "parser/file_parser.h"

#include "misc/geometry.h"

/**
 *  Controlla che il tipo di una variabile sia presente tra quelli riconosciuti
 *
 *  @param type Tipo da controllare
 *
 *  @return Identificativo del tipo della variabile
 *
 *  @retval CONFIG_VARTYPE_UNKNOWN Se il tipo non è riconosciuto
 */
config_variable_type config_check_variable_type(char * type)
{

    if (!strncasecmp(type, "int", 3))
        return CONFIG_VARTYPE_INT;

    if (!strncasecmp(type, "string", 6))
        return CONFIG_VARTYPE_STRING;

    if (!strncasecmp(type, "list", 4))
        return CONFIG_VARTYPE_LIST;

    if (!strncasecmp(type, "size", 4))
        return CONFIG_VARTYPE_SIZE;

    if (!strncasecmp(type, "rectangle", 9))
        return CONFIG_VARTYPE_RECTANGLE;

    if (!strncasecmp(type, "dictionary", 10))
        return CONFIG_VARTYPE_DICTIONARY;

    if (!strncasecmp(type, "float", 5))
        return CONFIG_VARTYPE_FLOAT;

    return CONFIG_VARTYPE_UNKNOWN;

}

/**
 *  Converte una dimensione da stringa ad un dimension_t
 *
 *  @code
 *  size nome_variabile = [800, 600];
 *  @endcode
 *
 *  @param parser Parser del file di configurazione
 *
 *  @return Dimensione
 */
dimension_t * config_read_size(parser_t * parser)
{

    //  [
    if (!parser_expect(parser, '['))
        return NULL;

    dimension_t * dimensions = memalloc(dimension_t);

    int index = 0;

    //  il ciclo termina quando si arriva ad un ]
    do {

        int current = (int)parser_long_integer(parser);

        //  w
        if (index == 0)
            dimensions->width = current;
        else if (index == 1)    //  h
            dimensions->height = current;

        index++;

        if (parser_expect(parser, ']')) {

            break;

        } else if (!parser_expect(parser, ',')) {

            errorf("Formato dimensione non valido, virgola mancante (offset: %zu)\n", parser->position);

            memfree(dimensions);
            return NULL;

        } else if (index > 1) {

            errorf("Formato dimensione non valido, elementi in eccesso (offset: %zu)\n", parser->position);

            memfree(dimensions);
            return NULL;

        }

    } while (1);

    return dimensions;

}

/**
 *  Converte una rettangolo da stringa ad un rectangle_t
 *
 *  @code
 *  rectangle nome_variabile = [0, 0, 100, 200];
 *  @endcode
 *
 *  @param parser Parser del file di configurazione
 *
 *  @return Rettangolo
 */
rectangle_t * config_read_rectangle(parser_t * parser)
{

    //  [
    if (!parser_expect(parser, '['))
        return NULL;

    rectangle_t * rect = memalloc(rectangle_t);

    int index = 0;

    //  il ciclo termina quando si arriva ad un ]
    do {

        int current = (int)parser_long_integer(parser);

        //  x
        if (index == 0)
            rect->origin.x = current;
        else if (index == 1)    //  y
            rect->origin.y = current;
        else if (index == 2)    //  w
            rect->size.width = current;
        else if (index == 3)    //  h
            rect->size.height = current;

        index++;

        if (parser_expect(parser, ']')) {

            break;

        } else if (!parser_expect(parser, ',')) {

            //  manca la virgola tra 2 elementi

            errorf("Formato rettangolo non valido, virgola mancante (offset: %zu)\n", parser->position);

            memfree(rect);
            return NULL;

        } else if (index > 3) {

            //  c'è un elemento di troppo

            errorf("Formato rettangolo non valido, elementi in eccesso (offset: %zu)\n", parser->position);

            memfree(rect);
            return NULL;

        }

    } while (1);

    return rect;

}

/**
 *  Converte una lista da stringa ad un list_t
 *
 *  @code
 *  list[int] nome_variabile = 1, 2, 3;
 *  @endcode
 *
 *  @param parser Parser del file di configurazione
 *  @param contents_type Tipo delle variabili contenute (qualsiasi tipo eccetto list e dictionary)
 *
 *  @return Lista contenente le variabili
 */
list_t * config_read_list(parser_t * parser, config_variable_type contents_type)
{

    struct type_functions list_functions;

    if (contents_type == CONFIG_VARTYPE_INT)
        list_functions = long_functions;
    else if (contents_type == CONFIG_VARTYPE_STRING)
        list_functions = string_functions;
    else if (contents_type == CONFIG_VARTYPE_RECTANGLE)
        list_functions = rectangle_functions;
    else if (contents_type == CONFIG_VARTYPE_SIZE)
        list_functions = dimension_functions;
    else if (contents_type == CONFIG_VARTYPE_FLOAT)
        list_functions = float_functions;

    //  creazione lista
    list_t * list = list_new(list_functions);

    void * value = NULL;

    //  il ciclo termina quando si arriva ad un ; e quindi value == NULL
    do {

        //  lettura del valore
        config_read_value(parser, contents_type, CONFIG_VARTYPE_UNKNOWN, &value, NULL);

        //  inserimento senza copia e con deallocazione
        list_insert(list, value, INSERT_MODE_TAIL, false, true);

        //  fine lista
        if (parser_expect(parser, ';')) {

            value = NULL;

        } else if (!parser_expect(parser, ',')) {

            //  manca la virgola tra un elemento e l'altro
            errorf("Formato lista non valido, virgola mancante (offset: %zu)\n", parser->position);

            list_delete(list);
            return NULL;

        }

    } while (value != NULL);

    return list;

}

/**
 *  Converte un dizionario da stringa ad una Hashtable
 *
 *  @code
 *  dictionary nome_variabile = {
 *      tipo_variabile nome_variabile_1 = valore;
 *      tipo_variabile nome_variabile_2 = valore;
 *      ...
 *  }
 *  @endcode
 *
 *  @param parser Parser del file di configurazione
 *
 *  @return Hashtable contenente le variabili del dizionario
 */
hashtable_t * config_read_dictionary(parser_t * parser)
{

    //  {
    if (!parser_expect(parser, '{'))
        return NULL;

    //  creazione nuova hashtable
    hashtable_t * ht = hashtable_new(HASHTABLE_DEFAULT_SIZE, string_functions);

    //  si cambia momentaneamente la destinazione del parsing
    //  con la hashtable appena allocata
    void * destination = parser->destination;
    parser->destination = ht;

    //  e si riprende come fosse un normale file di configurazione
    while (!parser->methods.is_eos(parser) && !parser_expect(parser, '}')) {

        int result = config_parsing(parser);

        if (result < 0)
            break;

        parser_expect(parser, ',');
        parser_trim(parser);

    }

    //  arrivati alla parentesi di chiusura si ripristina l'hashtable principale
    parser->destination = destination;

    return ht;

}

void config_read_value(
    parser_t * parser,
    config_variable_type type,
    config_variable_type contents_type,
    void ** value,
    struct type_functions * functions)
{

    if (type == CONFIG_VARTYPE_INT) {   //  è dichiarata come intero

        long lv = parser_long_integer(parser);

        *value = long_functions.duplicate(&lv);

        if (functions)
            *functions = long_functions;

    } else if (type == CONFIG_VARTYPE_STRING) {   //  è dichiarata come stringa

        *value = parser_string(parser);

        if (functions)
            *functions = string_functions;

    } else if (type == CONFIG_VARTYPE_LIST) {   //  è una lista

        *value = config_read_list(parser, contents_type);

        if (functions)
            *functions = list_functions;
        
    } else if (type == CONFIG_VARTYPE_RECTANGLE) {  //  rettangolo

        *value = config_read_rectangle(parser);

        if (functions)
            *functions = rectangle_functions;

    } else if (type == CONFIG_VARTYPE_SIZE) {   //  dimensioni

        *value = config_read_size(parser);

        if (functions)
            *functions = dimension_functions;

    } else if (type == CONFIG_VARTYPE_DICTIONARY) { //  dizionario

        *value = config_read_dictionary(parser);

        if (functions)
            *functions = hashtable_functions;

    } else if (type == CONFIG_VARTYPE_FLOAT) {  //  float

        float fv = parser_float(parser);

        *value = float_functions.duplicate(&fv);

        if (functions)
            *functions = float_functions;

    }

}

int config_parsing(parser_t * parser)
{

    //  commento
    while (parser->methods.current(parser) == '#') {
        while (parser->methods.current(parser) != '\n') {
            parser->methods.next(parser);
        }
        parser_trim(parser);
    }

    //  tipo della variabile: int, string, ...
    char * type_id = parser_alpha_identifier(parser);

    config_variable_type type;
    config_variable_type contents_type = CONFIG_VARTYPE_UNKNOWN;

    if (!type_id) {
        errorf("Errore di parsing: tipo non valido (offset: %zu)\n", parser->position);
        return -1;
    }

    //  controlliamo che il tipo sia tra quelli riconosciuti
    if ((type = config_check_variable_type(type_id)) == CONFIG_VARTYPE_UNKNOWN) {
        errorf("Errore di parsing: tipo di dato non riconosciuto %s (offset: %zu)\n", type_id, parser->position);
        return -2;
    }

    memfree(type_id);

    //  nel caso sia una lista, va individuato il tipo di elementi che contiene
    if (type == CONFIG_VARTYPE_LIST) {

        //  la lista è dichiarata come list[tipo dati contenuti]
        parser_expect(parser, '[');

        char * contents_type_id = parser_alpha_identifier(parser);

        if (!contents_type_id) {
            errorf("Errore di parsing: tipo non valido (offset: %zu)\n", parser->position);
            return -1;
        }

        //  controlliamo che il tipo sia tra quelli riconosciuti
        contents_type = config_check_variable_type(contents_type_id);

        //  per semplicità le liste di liste non sono supportate
        if (contents_type == CONFIG_VARTYPE_UNKNOWN || contents_type == CONFIG_VARTYPE_LIST) {
            errorf("Errore di parsing: tipo di dato non riconosciuto o non valido (offset: %zu)\n",parser->position);
            return -2;
        }

        parser_expect(parser, ']');

        memfree(contents_type_id);

    }

    //  nome della variabile
    char * identifier = parser_alpha_numeric_uscore_identifier(parser);

    if (!identifier) {
        errorf("Errore di parsing: impossibile individuare il nome della variabile (offset: %zu)\n", parser->position);
        return -3;
    }

    //  uguale
    if (!parser_expect(parser, '=')) {
        errorf("Errore di parsing: = mancante o mal posizionato (offset: %zu)\n", parser->position);
        return -4;
    }

    //  valore
    void * value = NULL;

    //  funzioni per il tipo selezionato
    struct type_functions functions = no_functions;

    config_read_value(parser, type, contents_type, &value, &functions);

    if (!value) {
        errorf("Errore di parsing: valore non valido (%s, offset: %zu)\n", identifier,  parser->position);
        memfree(identifier);
        return -5;
    }

    hashtable_t * table = parser->destination;

    //  inserimento con copia del valore se non si tratta di una lista
    hashtable_insert(table, identifier, value, functions, (type != CONFIG_VARTYPE_LIST && type != CONFIG_VARTYPE_DICTIONARY), true);

    //  deallocazione chiave e valore dato che sono stati copiati
    memfree(identifier);

    if (type != CONFIG_VARTYPE_LIST && type != CONFIG_VARTYPE_DICTIONARY)
        functions.delete(value);

    return 0;

}

/**
 *  Esegue il parsing di un file di configurazione
 *
 *  @param parser Parser del file di configurazione
 *
 *  @return Stato del parsing
 *
 *  @retval 0 in caso di parsing effettuato con successo
 *  @retval < 0 in caso di errori
 */
int config_parse(parser_t * parser)
{

    //  continua finchè il buffer non termina
    while (!parser->methods.is_eos(parser)) {

        int result = config_parsing(parser);

        if (result < 0)
            return result;

        parser_expect(parser, ';');
        parser_trim(parser);

    }

    return 0;

}

hashtable_t * config_open(char * file_path)
{

    //  file di conf
    FILE * config = fopen(file_path, "r");

    //  ci sono problemi nella lettura del file
    if (!config) {
        errorf("Errore in fase di lettura del file %s\n", file_path);
        return NULL;
    }

    hashtable_t * table = hashtable_new(HASHTABLE_DEFAULT_SIZE, string_functions);

    //  creazione parser con il file e la tabella
    parser_t * parser = parser_new(config, 0, table, file_parser_functions);

    int r;

    if ((r = config_parse(parser)) != 0) {

        errorf("Errore %d in fase di parsing del file %s\n", r, file_path);

        hashtable_delete(table);
        table = NULL;

    }

    //  deallocazione parser
    parser_delete(parser);

    fclose(config);

    return table;

}