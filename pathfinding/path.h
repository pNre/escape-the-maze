#ifndef pathfinding_path_h
#define pathfinding_path_h

#include "std/stack.h"

#include "main/color.h"

#include "game/structs.h"

/**
 *  Costruisce il percorso minimo dalla cella A alla cella A 
 *  e inserisce i nodi del percorso da A a A nello stack path
 *
 *  @param map Mappa
 *  @param a Cella A
 *  @param b Cella B
 *  @param path Stack che conterrà i nodi del percorso
 *
 *  @retval false Se non esiste un percorso
 *  @retval true Se è stato trovato un percorso
 */
bool shortest_path_find(map_t * map, map_cell_t * a, map_cell_t * b, sstack_t * path);

/**
 *  Disegna un percorso sulla mappa
 *
 *  @param path Stack contenente i nodi del percorso
 *  @param color Colore con il quale disegnare il percorso
 */
void path_draw(sstack_t * path, color_t color);

#endif
