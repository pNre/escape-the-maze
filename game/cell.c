#include "game/cell.h"

#include "utils.h"

#include "types.h"

//  Colore nodo
int cell_get_color(map_cell_t * cell)
{
    return cell->node.color;
}

void cell_set_color(map_cell_t * cell, int color)
{
    cell->node.color = color;
}

//  Peso nodo
int cell_get_value(map_cell_t * cell)
{
    return cell->node.value;
}

int cell_get_distance(map_cell_t * cell)
{
    return cell->node.distance;
}

//  Distanza dalla sorgente
void cell_set_distance(map_cell_t * cell, int distance)
{
    cell->node.distance = distance;
}

map_cell_t * cell_get_parent(map_cell_t * cell)
{
    return cell->node.parent;
}

//  Nodo genitore
void cell_set_parent(map_cell_t * cell, map_cell_t * parent)
{
    cell->node.parent = parent;
}

void cell_set_value(map_cell_t * cell, int value)
{
    cell->node.value = RangeNormalizedValue(CellValueRange, value);
}

//  Tipo di nodo
void cell_set_type(map_cell_t * cell, int type)
{
    cell->type = type;
}

point_t cell_location_to_position(point_t location)
{

    return PointMake(CellSize.width * location.x, CellSize.height * location.y);

}

point_t cell_position_to_location(point_t position)
{

    return PointMake(floorf(position.x / CellSize.width), floorf(position.y / CellSize.height));

}

color_t cell_tint(int cell_value)
{
    
    unsigned char value;
    
    if (cell_value == CellDefaultValue)
        return ColorMakeRGBA(255, 255, 255, 0);
    
    if (cell_value < CellDefaultValue) {
        value = 64 - (16 * (cell_value - 1));
        return ColorMakeRGBA(value, value, value, 255);
    }
    
    value = 96 + 16 * (10 - cell_value);
    return ColorMakeRGBA(0, 0, 0, value);
    
}

unsigned int cell_get_adjacency(map_cell_t * cell, map_cell_t * adjacency[4])
{

    unsigned int length = 0;

    map_cell_t * adj[] = {

        cell->adjacency.north,
        cell->adjacency.south,
        cell->adjacency.east,
        cell->adjacency.west

    };

    unsigned int i;
    for (i = 0; i < sizeof(adj) / sizeof(map_cell_t *); i++) {
        if (adj[i])
            adjacency[length++] = adj[i];
    }

    return length;

}

void cell_init(map_cell_t * cell, point_t location)
{

    //  tipo: muro/corridoio
    cell->type = CELL_TYPE_UNKNOWN;

    //  coordinate della cella sulla mappa
    cell->location = location;

    //  Adiacenza della cella, vista la struttura della mappa
    //  e la capacità dei personaggi di muoversi in 4 direzioni
    //  ogni cella può avere al massimo 4 celle adiacenti
    cell->adjacency.north = NULL;
    cell->adjacency.south = NULL;
    cell->adjacency.east = NULL;
    cell->adjacency.west = NULL;

    //  Dati per il nodo del grafo rappresentato dalla cella
    cell->node.parent = NULL;
    cell->node.color = CELL_COLOR_WHITE;
    cell->node.value = CellDefaultValue;

    //  bonus
    cell->powerup.powerup = NULL;
    cell->powerup.place_time = time(NULL);

}
