#ifndef game_map_h
#define game_map_h

#include <stdlib.h>

#include "misc/geometry.h"

#include "game/level.h"
#include "game/cell.h"
#include "game/structs.h"

#include "std/queue.h"

/**
 *  Contiene le informazioni su una mappa
 */
struct map_s {

    /** Dimensione, in celle, della mappa */
    dimension_t size;

    /** Offset dello schermo al quale disegnare la mappa */
    point_t offset;

    /** Coordinate della cella di ingresso della mappa */
    point_t start;

    /** Coordinate della cella di uscita della mappa */
    point_t end;

    /** Griglia delle celle */
    map_cell_t ** grid;

    /** Probabilità che una cella della mappa possa contenere un bonus (0. = nessuna) */
    float powerup_probability;

    /** Tempo di attesa minimo, in secondi, tra la comparsa di due bonus */
    int powerups_time;

    /** Lista delle celle nelle quali può comparire un bonus */
    list_t * powerup_cells;

    /** Orario dell'inserimento dell'ultimo bonus */
    time_t powerup_time;

    /** Numero massimo di bonus che possono apparire sulla mappa contemporaneamente */
    long powerups_limit;

    /** Prossima mappa nel livello */
    struct map_s * next;

};

/**
 *  Accesso ad una cella di una mappa
 *
 *  @param map Mappa
 *  @param point Coordinate della cella
 *
 *  @return Cella
 */
map_cell_t * map_get_cell(map_t * map, point_t location);

/**
 *  Verifica la validità delle coordinate di una cella sulla mappa
 *
 *  @param map Mappa
 *  @param point Coordinate della cella
 *
 *  @retval true Cella valida
 *  @retval false Cella non valida
 */
sinline bool map_cell_is_valid(map_t * map, point_t location)
{
    
    dimension_t size = map->size;
    
    return  RangeContainsValue(RangeMake(0, size.width - 1), location.x) &&
            RangeContainsValue(RangeMake(0, size.height - 1), location.y);
    
}

/**
 *  Verifica la calpestabilità di una cella della mappa date le sue coordinate
 *
 *  @param map Mappa
 *  @param point Coordinate della cella
 *
 *  @retval true Cella calpestabile
 *  @retval false Cella non calpestabile
 */
#define map_cell_is_path(map, point)        (map &&     \
                                             map_cell_is_valid(map, point) &&   \
                                             cell_is_path(map_get_cell(map, point)))

/**
 *  Converte le coordinate di una cella (x, y) in un singolo indice
 *
 *  @param map Mappa
 *  @param point Coordinate della cella
 *
 *  @return Indice
 */
unsigned int map_cell_location_to_index(map_t * map, point_t point);

/**
 *  Creazione di una nuova mappa vuota
 *
 *  @param size Dimensione della mappa in celle
 *  @param level Livello al quale apparterrà la mappa
 *
 *  @return Mappa
 */
map_t * map_new(dimension_t size, level_t * level);

/**
 *  Creazione di una nuova mappa a partire dal parsing di un file di configurazione
 *
 *  @param level Livello al quale apparterrà la mappa
 *  @param path Percorso del file di configurazione
 *
 *  @return Mappa
 */
map_t * map_load_new(level_t * level, char * path);

/**
 *  Genera una mappa in maniera casuale e la associa ad un certo livello
 *
 *  @param level Livello al quale associare la mappa
 *  @param size Dimensioni della mappa, in celle, considerando solo i corridoi
 *  @param start_on_x Se falso posiziona l'entrata sull'asse y
 *  @param randomize_weights Se assegnare ad ogni cella un peso casuale
 *  @param type Tipo di labirinto (RANDOM_BRAID -> labirinto senza vicoli ciechi, RANDOM -> labirinto perfetto)
 *
 *  @return Mappa generata casualmente
 */
map_t * map_generate(level_t * level, dimension_t size, bool start_on_x, bool randomize_weights, char * type);

/**
 *  Posiziona gli avversari in posizioni random della mappa
 *
 *  @param level Livello della mappa
 *  @param map Mappa
 */
void map_place_enemies(level_t * level, map_t * map);

/**
 *  Deallocazione di una mappa
 *
 *  @param map Mappa da deallocare
 */
void map_delete(map_t * map);

/**
 *  Inizializza il grafo della mappa ripristinando i valori di default delle proprietà:
 *  - parent
 *  - color
 *  - distance
 *  di ogni nodo
 *
 *  @param map Mappa
 */
void map_clear_graph(map_t * map);

/**
 *  Calcolo dell'offset della mappa sullo schermo
 *
 *  @param map Mappa della quale calcolare l'offset
 */
void map_offset_calculate(map_t * map);

/**
 *  In fase di inizializzazione posiziona un personaggio in una cella valida della mappa
 *
 *  @param map Mappa
 *  @param character Personaggio da posizionare
 */
void map_dispose_charcter(map_t * map, character_t * character);

/**
 *  Effettua le connessioni tra ogni cella delle mappa e le sue 4 adiacenze
 *  per costurire il grafo della mappa
 *
 *  @param map Mappa
 */
void map_connect(map_t * map);

/**
 *  Effettua le connessioni tra una cella delle mappa e le sue 4 adiacenze
 *
 *  @param map Mappa
 *  @param cell Cella da connettere
 *  @param all_directions Se falso collega la cella solo a nord e ovest
 */
void map_connect_cell(map_t * map, map_cell_t * cell, bool all_directions);

/**
 *  Aggiorna (ridisegna) una mappa e la legenda
 *
 *  @param game Contesto di gioco
 *  @param map Mappa
 */
void map_update(game_t * game, map_t * map);

/**
 *  Ricerca una cella casuale di tipo corridoio nella mappa
 *
 *  @param map Mappa
 *
 *  @return Cella di tipo corridoio
 */
map_cell_t * map_random_cell_path(map_t * map);

/**
 *  Controlla se un punto della mappa è sui confini
 *
 *  @param map Mappa
 *  @param point Punto
 */
sinline bool map_point_is_on_borders(map_t * map, point_t point) {
    
    return !((point.x > 0 && point.x < map->size.width - 1) &&
             (point.y > 0 && point.y < map->size.height - 1));
    
}

#endif
