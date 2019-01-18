#include <stdio.h>
#include <math.h>

#include "utils.h"

#include "parser/parser.h"

#include "config/config.h"

#include "misc/random.h"
#include "misc/geometry.h"

#include "game/map.h"
#include "game/character.h"
#include "game/game.h"
#include "game/info.h"

#include "main/drawing.h"
#include "main/graphics.h"
#include "main/tiling.h"

/**
 *  Aggiorna (ridisegna) una mappa e la legenda
 *
 *  @param game Contesto di gioco
 *  @param map Mappa
 */
void map_update(game_t * game, map_t * map)
{

    //  traslazione dell'origine
    output_set_origin(map->offset);

    //  disegno della mappa
    output_map(game_get_current_level(game), map);

    //  legenda
    info_update(game);

}

void map_clear_graph(map_t * map)
{

    int x, y;

    dimension_t size = map->size;

    for (x = 0; x < size.width; x++) {
        for (y = 0; y < size.height; y++) {
            map_cell_t * cell = map_get_cell(map, PointMake(x, y));
            cell_set_color(cell, CELL_COLOR_WHITE);
            cell_set_distance(cell, INT_MAX);
            cell_set_parent(cell, NULL);
        }
    }

}

void map_offset_calculate(map_t * map)
{

    dimension_t screen_size = graphics_get_screen_size();
    int y = 0, x = 0;

    //  la mappa va al centro dello schermo
    x = (screen_size.width - (map->size.width * CellSize.width)) / 2;
    y = (screen_size.height - (map->size.height * CellSize.height)) / 2;

    //  offset al quale disegnare la mappa
    map->offset = PointMake(x, y);

}

/**
 *  Aggiorna le dimensioni di una mappa
 *
 *  @param map Mappa
 *  @param size Nuove dimensioni
 */
void map_update_size(map_t * map, dimension_t size) {
 
    dimension_t old_size = map->size;
    
    //  aggiornamento dimensioni
    map->size = size;
    
    //  deallocazione delle locazioni in eccesso
    int y = map->size.height;
    for (; y < old_size.height; y++) {
        memfree(map->grid[y]);
    }
    
}

/**
 *  Esegue il parsing di una mappa, data la sua rappresentazione testuale costituita dai seguenti elementi:
 *  - '#': muro
 *  - ' ': corridoio
 *  - '@': corridoio a percorrenza lenta
 *  - 'V': posizione di un avversario
 *  - 'S': punto partenza
 *  - 'E': punto di uscita
 *
 *  @param level Livello
 *  @param map Mappa di destinazione
 *  @param buffer Rappresentazione testuale della mappa
 *
 *  @retval true Parsing terminato senza errori
 *  @retval false Parsing fallito
 */
bool map_parse(level_t * level, map_t * map, char * buffer)
{

    //  nuovo parser
    parser_t * parser = parser_new(buffer, strlen(buffer), NULL, buffer_parser_functions);

    //  coordinate del punto nel quale si trova il parser ad un certo loop
    point_t point = PointZero;

    //  primo avversario nella lista degli avversari, per l'assegnazione della posizione
    list_node_t * enemy_node = level->enemies->head;

    while (!parser->methods.is_eos(parser)) {

        int token = parser->methods.current(parser);
        map_cell_t * cell = map_get_cell(map, point);

        //  tokens:
        //  '#': muro
        //  ' ': corridoio
        //  'V': posizione di un avversario
        //  '2'..'9': cella con un peso diverso (i personaggi la attraversano più lentamente)
        //  'S': partenza
        //  'E': fine
        switch (token) {
        case '#':
            cell_set_is_wall(cell);
            point.x++;
            break;

        case 'P':
        case 'S':
        case 'E':
        case 'V':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case ' ':

            if (token == 'S')   //  inizio della mappa == posizione iniziale del personaggio dell'utente
                map->start = point;
            else if (token == 'E')  //  punto di uscita della mappa
                map->end = point;
            else if (token == 'V' && enemy_node) {    //  posizione di un avversario
                character_t * enemy = enemy_node->value;

                //  se il personaggio è valido e non gli è stata assegnata alcuna posizione
                if (enemy && PointIsNull(enemy->location)) {
                    enemy->location = point;
                    enemy->position = cell_location_to_position(point);
                    enemy->map = map;

                    //  prossimo personaggio
                    enemy_node = enemy_node->next;
                }
            } else if (token == 'P' ||
                       (token == ' ' && random_bool(map->powerup_probability))) {
                list_insert(map->powerup_cells, cell);
            }

            //  corridoio a percorrenza lenta
            if (isdigit(token)) {
                cell_set_value(cell, token - '0');
            } else {
                cell_set_value(cell, CellDefaultValue);
            }

            cell_set_is_path(cell);

            point.x++;
            break;

        case '\n':
            point.x = 0;
            point.y++;
            break;

        default:
            break;
        }

        //  avanti il prossimo
        parser->methods.next(parser);
        
        //  fuori dai limiti
        if (point.x >= map->size.width || point.y >= map->size.height) {
            
            //  deallocazione parser
            parser_delete(parser);

            return false;
        }

    }

    //  deallocazione parser
    parser_delete(parser);

    //  controlla se sono state specificate posizione di inizio e fine
    if (PointIsNull(map->start) || PointIsNull(map->end))
        return false;

    //  aggiornamento dimensioni
    map_update_size(map, SizeMake(point.x, point.y + 1));

    //  se la mappa è più piccola della dimensione della finestra, va centrata
    map_offset_calculate(map);

    return true;

}

map_t * map_new(dimension_t size, level_t * level)
{

    //  oggetto map_t
    map_t * map = memalloc(map_t);

    //  punto di partenza e fine
    //  inizializzati con una posizione "impossibile" per forzarne l'indicazione sulla mappa
    map->start = map->end = PointNull;

    //  copia dimensioni
    map->size = size;

    //  a questo punto è possibile allocare la griglia
    //  righe
    map->grid = memalloc(map_cell_t *, map->size.height);

    //  inizializzazione delle celle
    int x, y;

    for (y = 0; y < map->size.height; y++) {
        map->grid[y] = memalloc(map_cell_t, map->size.width);
        for (x = 0; x < map->size.width; x++) {
            //  inizializzazione cella
            cell_init(map_get_cell(map, PointMake(x, y)), PointMake(x, y));
        }
    }

    //  il puntatore alla mappa successiva sarà aggiornato dopo il caricamento
    //  di tutte le mappe del livello
    map->next = NULL;

    //  lista delle celle per i bonus
    map->powerup_cells = list_new(no_functions);

    //  ultimo bonus inserito mai
    struct tm time;
    memset(&time, 0, sizeof(time));
    map->powerup_time = mktime(&time);

    return map;

}

void map_connect_cell(map_t * map, map_cell_t * cell, bool all_directions)
{

    if (!map)
        return;
    
    //  point è la locazione della cella corrente
    //  north e la cella in alto
    //  west quella a sinistra
    point_t point = cell->location;
    point_t north = PointPointOffset(point, OffsetTop);
    point_t west  = PointPointOffset(point, OffsetLeft);

    //  definizione adiacenza
    //  ovest
    if (map_cell_is_path(map, west)) {
        map_get_cell(map, west)->adjacency.east = cell;
        cell->adjacency.west = map_get_cell(map, west);
    }

    //  nord
    if (map_cell_is_path(map, north)) {
        map_get_cell(map, north)->adjacency.south = cell;
        cell->adjacency.north = map_get_cell(map, north);
    }

    if (!PointEqualToPoint(point, map->start) &&
        !PointEqualToPoint(point, map->end)) {
        //  sconfinamento a est
        if (cell->location.x == map->size.width - 1 && map_cell_is_path(map, PointMake(0, cell->location.y))) {
            map_get_cell(map, PointMake(0, cell->location.y))->adjacency.west = cell;
            cell->adjacency.east = map_get_cell(map, PointMake(0, cell->location.y));
        }

        //  sconfinamento a sud
        if (cell->location.y == map->size.height - 1 && map_cell_is_path(map, PointMake(cell->location.x, 0))) {
            map_get_cell(map, PointMake(cell->location.x, 0))->adjacency.north = cell;
            cell->adjacency.south = map_get_cell(map, PointMake(cell->location.x, 0));
        }
    }

    if (all_directions) {

        point_t south = PointPointOffset(point, OffsetBottom);
        point_t east  = PointPointOffset(point, OffsetRight);

        //  sud
        if (map_cell_is_path(map, south)) {
            map_get_cell(map, south)->adjacency.north = cell;
            cell->adjacency.south = map_get_cell(map, south);
        }

        //  est
        if (map_cell_is_path(map, east)) {
            map_get_cell(map, east)->adjacency.west = cell;
            cell->adjacency.east = map_get_cell(map, east);
        }

        if (!PointEqualToPoint(point, map->start) && !PointEqualToPoint(point, map->end)) {
            //  sconfinamento a est
            if (cell->location.x == 0 && map_cell_is_path(map, PointMake(map->size.width - 1, cell->location.y))) {
                map_get_cell(map, PointMake(map->size.width - 1, cell->location.y))->adjacency.east = cell;
                cell->adjacency.west = map_get_cell(map, PointMake(map->size.width - 1, cell->location.y));
            }

            //  sconfinamento a sud
            if (cell->location.y == 0 && map_cell_is_path(map, PointMake(cell->location.x, map->size.height - 1))) {
                map_get_cell(map, PointMake(cell->location.x, map->size.height - 1))->adjacency.south = cell;
                cell->adjacency.north = map_get_cell(map, PointMake(cell->location.x, map->size.height - 1));
            }
        }

    }

}

void map_connect(map_t * map)
{

    //  inizializzazione delle adiacenze delle celle
    //  che saranno utilizzate per calcolare i percorsi con bfs, astar, dijkstra
    int x, y;
    for (y = 0; y < map->size.height; y++) {
        for (x = 0; x < map->size.width; x++) {

            map_cell_t * cell = map_get_cell(map, PointMake(x, y));

            //  le mura non vanno connesse
            if (!cell_is_path(cell))
                continue;

            map_connect_cell(map, cell, 0);

        }
    }

}

map_t * map_load_new(level_t * level, char * config_file_path)
{

    //  lettura del file di configurazione
    hashtable_t * map_config = config_open(config_file_path);

    //  non problemi nel file di config
    if (!map_config)
        return NULL;

    //  il file di configurazione deve contenere informazioni sulla dimensione
    dimension_t * map_size = hashtable_search(map_config, "size");

    //  se non ci sono è impossibile proseguire
    if (!map_size) {
        return NULL;
    }

    //  creazione mappa
    map_t * map = map_new(*map_size, level);

    //  probabilità che una cella contenga un potenziamento
    float * powerup_probability = hashtable_search(map_config, "powerup_probability");
    map->powerup_probability = powerup_probability ? *powerup_probability : 0.05;

    //  tempo di attesa tra i bonus
    float * powerups_time = hashtable_search(map_config, "powerups_time");
    map->powerups_time = powerups_time ? *powerups_time : 10.;

    //  limite di bonus
    long * powerups_limit = hashtable_search(map_config, "powerups_limit");
    map->powerups_limit = powerups_limit ? *powerups_limit : 5;

    //  caricamento della struttura della mappa
    char * map_struct = hashtable_search(map_config, "map");

    debugf("[Mappa] %s caricata:\n%s\n", config_file_path, map_struct);

    //  parsing
    if (!map_parse(level, map, map_struct)) {
        //  errore di parsing
        debugf("Errore nel parsing della mappa.\n", NULL);
        map_delete(map);
        map = NULL;
    }

    //  inizializzazione delle adiacenze delle celle
    map_connect(map);
    
    hashtable_delete(map_config);

    return map;

}

void map_delete(map_t * map)
{

    //  1. deallocazione griglia
    int y;
    for (y = 0; y < map->size.height; y++) {
        memfree(map->grid[y]);
    }

    memfree(map->grid);

    //  2. lista delle celle
    list_delete(map->powerup_cells);

    //  3. deallocazione mappa
    memfree(map);

}

map_cell_t * map_get_cell(map_t * map, point_t location)
{

    return &map->grid[(int)location.y][(int)location.x];

}

map_cell_t * map_random_cell_path(map_t * map)
{

    point_t point = PointZero;
    map_cell_t * cell;

    do {
        point.x = random_int(0, (int)map->size.width - 1);
        point.y = random_int(0, (int)map->size.height - 1);
        cell = map_get_cell(map, point);
    } while (!cell_is_path(cell));

    return cell;

}

/**
 *  Assegna dei pesi casuali ad un numero casuale di celle di una mappa
 *
 *  @param map Mappa
 *  @param complexity Complessità della mappa (determina quanti percorsi pesanti/leggeri creare)
 */
void map_generate_randomize_weights(map_t * map, float complexity)
{

    //  numero di corridoi a peso diverso da creare
    int max = random_int(0, (int)(10 * complexity));
    
    for (; max >= 0; --max) {
        
        //  lunghezza del percorso con peso diverso da quello standard
        int length = random_int(1, 5);

        //  cella iniziale
        map_cell_t * cell = map_random_cell_path(map);

        //  se la cella non ha già un peso diverso
        if (cell_get_value(cell) == CellDefaultValue) {

            //  si sceglie una direzione casuale
            point_t offsets[] = {
                OffsetTop,
                OffsetLeft,
                OffsetRight,
                OffsetBottom
            };

            point_t offset;

            //  si cerca un percorso in una direzione casuale
            do {
                offset = offsets[random_int(0, sizeof(offsets) / sizeof(point_t) - 1)];
            } while (!map_cell_is_path(map, PointPointOffset(cell->location, offset)));

            //  valore da utilizzare
            int range_size = ceilf(complexity * RangeSize(CellValueRange) / 2.);
            int value = random_int_in_range(RangeFromRangeCenter(CellValueRange, range_size));

            //  per ogni cella fino alla lunghezza desiderata si cambia valore
            point_t current = cell->location;
            for (; length >= 0; --length) {

                if (map_cell_is_path(map, current)) {
                    cell_set_value(map_get_cell(map, current), value);
                } else
                    break;

                current = PointPointOffset(current, offset);

            }

        }

    }

}

/**
 *  Genera un labirinto perfetto "scavando" una mappa costituita completamente da mura
 *
 *  @param map Mappa
 *  @param deadends Lista dei vicoli ciechi
 */
void map_generate_structure_perfect(map_t * map, queue_t * deadends)
{

    //  cella di partenza
    map_cell_t * cell = map_get_cell(map, PointMake(1, 1));

    queue_t * Q = queue_new();
    queue_push(Q, cell);

    while (!queue_empty(Q)) {

        const point_t offsets[4][2] = {
            { PointMultiply(OffsetRight,    2.), OffsetRight },
            { PointMultiply(OffsetLeft,     2.), OffsetLeft },
            { PointMultiply(OffsetBottom,   2.), OffsetBottom },
            { PointMultiply(OffsetTop,      2.), OffsetTop }
        };

        point_t points[4][2];

        unsigned int available = 0;

        //  si controlla se intorno alla cella in oggetto (a distanza 2)
        //  c'è una cella di tipo muro, in quel caso si salva l'offset
        unsigned int i;
        for (i = 0; i < 4; i++) {
            point_t point = PointPointOffset(cell->location, offsets[i][0]);
            if (map_cell_is_valid(map, point) && !cell_is_path(map_get_cell(map, point))) {
                points[available][0] = point;
                points[available][1] = PointPointOffset(cell->location, offsets[i][1]);
                available++;
            }
        }

        //  se è stata trovata almeno una cella
        if (available > 0) {

            //  se ne seleziona una a caso
            available = random_int(0, available - 1);

            cell = map_get_cell(map, points[available][0]);

            //  si crea un percorso
            cell_set_is_path(cell);
            cell_set_is_path(map_get_cell(map, points[available][1]));

            //  la cella potrebbe contenere dei bonus
            if (random_bool(map->powerup_probability)) {
                list_insert(map->powerup_cells, cell);
            }

            queue_push(Q, cell);

        } else {

            //  siamo in un vicolo cieco

            //  se è necessario salvare il nodo, si salva
            if (deadends)
                queue_push(deadends, cell);

            //  si passa al nodo successivo nella coda
            cell = (map_cell_t *)queue_pop(Q);

        }

    }

    queue_delete(Q);

}

/**
 *  Genera un labirinto perfetto ed elimina alcuni dei vicoli ciechi per rendere la mappa più interessante
 *
 *  @param map Mappa
 *  @param deadends_probability Proabilità che un vicolo cieco sia eliminato
 */
void map_generate_structure_braid(map_t * map, float deadends_probability)
{

    //  lista delle celle che fanno parte di un vicolo cieco
    queue_t * deadends = queue_new();

    //  si genera prima un labritino perfetto
    map_generate_structure_perfect(map, deadends);

    //  poi si controllano i vicoli ciechi
    while (!queue_empty(deadends)) {

        map_cell_t * cell = (map_cell_t *)queue_pop(deadends);

        //  se il caso è benevolo si procede, altrimenti si lascia così com è
        if (!random_bool(deadends_probability))
            continue;

        //  posizioni delle celle da controllare, relativamente alla corrente "cell"
        const point_t offsets[] = {
            OffsetTop,
            OffsetBottom,
            OffsetLeft,
            OffsetRight
        };

        point_t off[array_count(offsets)];
        unsigned int valid = 0;

        //  si controllano le celle nei paraggi in cerca di mura
        unsigned int i;
        for (i = 0; i < array_count(offsets); i++) {

            point_t p = PointPointOffset(cell->location, offsets[i]);

            //  le mura delle celle sui margini non vengono abbattute
            if (p.x == 0 || p.y == 0 || p.x == map->size.width - 1 || p.y == map->size.height - 1)
                continue;

            map_cell_t * cell = map_get_cell(map, p);

            if (map_cell_is_valid(map, p) && !cell_is_path(cell))
                off[valid++] = offsets[i];
        }

        //  nei paraggi ci sono solo percorsi
        if (!valid)
            continue;

        //  in una direzione casuale di quelle scelte
        point_t offset = off[random_int(0, valid - 1)];

        //  si abbatte il muro
        cell_set_is_path(map_get_cell(map, PointPointOffset(cell->location, offset)));

    }

    queue_delete(deadends);

}

void map_place_enemies(level_t * level, map_t * map) {
    
    //  gli avversari tendono ad essere posizionati al centro della mappa
    point_t point = PointMake(floor(map->size.width / 2), floor(map->size.height / 2));
    
    //  distanza massima dal centro
    unsigned int max_distance = (int)list_length(level->enemies) + 3;
    
    //  posizionamento avversari
    foreach(level->enemies, character_t *, enemy) {
        
        if (!PointIsNull(enemy->position) && !PointEqualToPoint(enemy->location, map->start))
            continue;
        
        //  ricerca di una cella casuale che non coincida con le celle di inizio e fine
        //  e che non sia troppo vicina all'uscita
        map_cell_t * enemy_cell;
        
        do {
            
            enemy_cell = map_random_cell_path(map);
            
        } while (Manhattan(point, enemy_cell->location) > max_distance ||
                 PointEqualToPoint(map->end, enemy_cell->location) ||
                 PointEqualToPoint(map->start, enemy_cell->location) ||
                 cell_get_color(enemy_cell) == CELL_COLOR_BLACK);
        
        //  assegnazione mappa e posizione
        enemy->map = map;
        
        //  colora la cella temporaneamente di nero, in modo da non aggiungere altri personaggi
        cell_set_color(enemy_cell, CELL_COLOR_BLACK);
        
        //  posiziona il personaggio
        character_set_location(enemy, enemy_cell->location, true);
    }
    
}

map_t * map_generate(level_t * level, dimension_t size, bool start_on_x, bool randomize_weights, char * type)
{

    size = SizeMake(size.width * 2 + 1, size.height * 2 + 1);

    //  creazione di una mappa tutte mura
    map_t * map = map_new(size, level);

    //  inizializzazione della mappa, inizialmente ogni cella è un muro
    int x, y;
    for (y = 0; y < size.height; y++) {
        for (x = 0; x < size.width; x++) {
            cell_set_is_wall(map_get_cell(map, PointMake(x, y)));
        }
    }

    //  probabilità che in una cella possa essere inserito un bonus
    //  più è complessa meno sono i bonus
    map->powerup_probability = 0.05 / level->complexity;

    //  tipo di mappa random da generare
    if (!strcasecmp(type, "RANDOM PERFECT"))
        map_generate_structure_perfect(map, NULL);
    else
        map_generate_structure_braid(map, 0.2);
    
    if (randomize_weights)
        map_generate_randomize_weights(map, level->complexity);
    
    //  calcolo del punto di ingresso della mappa (in base al punto di uscita della precedente)
    point_t points[] = {PointZero, PointZero, PointZero, PointZero};

    if (!start_on_x) {

        points[0].x = random_int(1, (int)map->size.width - 2);
        points[0].y = map->size.height - 1;

        points[1].x = points[0].x;
        points[1].y = points[0].y - 1;

        points[2].x = random_int(1, (int)map->size.width - 2);
        points[2].y = 0;

        points[3].x = points[2].x;
        points[3].y = 1;

    } else {

        points[0].x = 0;
        points[0].y = random_int(1, (int)map->size.height - 2);

        points[1].x = 1;
        points[1].y = points[0].y;

        points[2].x = map->size.width - 1;
        points[2].y = random_int(1, (int)map->size.height - 2);

        points[3].x = points[2].x - 1;
        points[3].y = points[2].y;

    }

    unsigned int i;
    for (i = 0; i < array_count(points); i++) {
        cell_set_is_path(map_get_cell(map, points[i]));
    }

    map->start = points[0];
    map->end = points[2];

    //  numero di bonus che possono comparire sulla mappa contemporaneamente
    map->powerups_limit = random_int(1, 7);

    //  attesa tra 2 bonus
    map->powerups_time = (int)(12 * level->complexity);
    if (map->powerups_time < 2)
        map->powerups_time = 2;

    //  se la mappa è più piccola della dimensione della finestra, va centrata
    map_offset_calculate(map);

    //  adiacenze
    map_connect(map);

    return map;
}

unsigned int map_cell_location_to_index(map_t * map, point_t point)
{

    dimension_t map_size = map->size;

    return (int)(point.y * map_size.width + point.x);

}

TYPE_FUNCTIONS_DEFINE(map, map_delete);
