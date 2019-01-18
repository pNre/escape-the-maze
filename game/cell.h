#ifndef game_cell_h
#define game_cell_h

#include "main/color.h"

#include "misc/geometry.h"
#include "misc/range.h"

#include "game/powerup.h"
#include "game/structs.h"

/**
 *  Definisce i possibili tipi di cella
 */
enum {

    CELL_TYPE_UNKNOWN,
    CELL_TYPE_PATH,
    CELL_TYPE_WALL,

    CELL_TYPE_LAST

};

/**
 *  Definisce il colore assegnato alla cella quando si esplora il grafo 
 *  sottostante la mappa
 */
enum {


    CELL_COLOR_WHITE,
    CELL_COLOR_GRAY,
    CELL_COLOR_BLACK

};

/**
 *  Contiene le informazioni su una cella della mappa
 */
struct map_cell_s {

    /**
     *  Tipo di cella:
     *  - CELL_TYPE_UNKNOWN
     *  - CELL_TYPE_PATH
     *  - CELL_TYPE_WALL
     */
    unsigned int type;

    /** Coordinate della cella nella mappa di appartenenza */
    point_t location;

    /** Bonus associato alla cella */
    powerup_location_t powerup;

    /** Adiacenze della cella, celle a N/S/E/O */
    struct {

        struct map_cell_s * north;
        struct map_cell_s * east;
        struct map_cell_s * west;
        struct map_cell_s * south;

    } adjacency;

    /** Informazioni sulla cella vista come nodo del grafo */
    struct {

        /** Cella genitore nell'esplorazione del grafo */
        struct map_cell_s * parent;

        /** Colore */
        unsigned int color;

        /** Distanza dalla sorgente della visita */
        int distance;

        /** Peso degli archi entranti */
        int value;

    } node;

};

/**
 *  Intervallo di valori che una cella può assumere
 */
#define CellValueRange  RangeMake(1, 9)

/**
 *  Valore di default
 */
#define CellDefaultValue    5

/**
 *  Dimensione di una cella in pixel
 */
#define CellSize SizeMake(32, 32)

/**
 *  Verifica che una cella sia di tipo corridoio
 *
 *  @param cell Cella da controllare
 *
 *  @retval true La cella è un corridoio
 *  @retval false La cella è di un tipo diverso da CELL_TYPE_PATH
 */
#define cell_is_path(cell)     (cell->type == CELL_TYPE_PATH)

/**
 *  Fornisce l'accesso al colore di una cella
 *
 *  @param cell Cella
 *
 *  @return Colore
 */
int cell_get_color(map_cell_t * cell);

/**
 *  Imposta il colore di una cella
 *
 *  @param cell Cella
 *  @param color Colore
 */
void cell_set_color(map_cell_t * cell, int color);

/**
 *  Fornisce l'accesso al genitore di una cella
 *
 *  @param cell Cella
 *
 *  @return Genitore
 */
map_cell_t * cell_get_parent(map_cell_t * cell);

/**
 *  Imposta il genitore di una cella
 *
 *  @param cell Cella
 *  @param parent Genitore
 */
void cell_set_parent(map_cell_t * cell, map_cell_t * parent);

/**
 *  Fornisce l'accesso alla distanza di una cella dalla sorgente della visita
 *
 *  @param cell Cella
 *
 *  @return Distanza
 */
int cell_get_distance(map_cell_t * cell);

/**
 *  Imposta la distanza di una cella dalla sorgente della visita
 *
 *  @param cell Cella
 *  @param distance Distanza
 */
void cell_set_distance(map_cell_t * cell, int distance);

/**
 *  Fornisce l'accesso al peso degli archi entranti in una cella
 *
 *  @param cell Cella
 *
 *  @return Valore
 */
int cell_get_value(map_cell_t * cell);

/**
 *  Costruisce la lista di adiacenza di una cella
 *
 *  @param cell Cella
 *  @param adjacency Destinazione della lista di adiacenza
 *
 *  @return Lunghezza della lista
 */
unsigned int cell_get_adjacency(map_cell_t * cell, map_cell_t * adjacency[4]);

/**
 *  Imposta il valore di una cella
 *
 *  @param cell Cella
 *  @param value Valore [1-9]
 */
void cell_set_value(map_cell_t * cell, int value);

/**
 *  Imposta il tipo di una cella
 *
 *  @param cell Cella
 *  @param type Tipo
 */
void cell_set_type(map_cell_t * cell, int type);

/**
 *  Rende una cella del tipo: corridoio
 *
 *  @param cell Cella
 */
#define cell_set_is_path(cell)  cell_set_type(cell, CELL_TYPE_PATH)

/**
 *  Rende una cella del tipo: mura
 *
 *  @param cell Cella
 */
#define cell_set_is_wall(cell)  cell_set_type(cell, CELL_TYPE_WALL)

/**
 *  Converte le coordinate di una cella in coordinate assolute (in pixel)
 *
 *  @param point Coordinate della cella
 *
 *  @return Coordinate assolute
 */
point_t cell_location_to_position(point_t location);

/**
 *  Converte le coordinate assolute (in pixel) di una cella in coordinate (in celle)
 *
 *  @param point Coordinate della cella
 *
 *  @return Coordinate in celle
 */
point_t cell_position_to_location(point_t position);

/**
 *  Inizializza le proprietà di default di una cella di una mappa
 *
 *  @param cell Cella
 *  @param location Posizione della cella sulla mappa
 */
void cell_init(map_cell_t * cell, point_t location);

/**
 *  Calcola la colorazione di una cella in base al valore
 *
 *  @param cell_value Valore della cella
 */
color_t cell_tint(int cell_value);

#endif
