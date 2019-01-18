#ifndef pathfinding_astar_h
#define pathfinding_astar_h

#include "game/structs.h"

/**
 *  Esplora il grafo di una mappa utilizzando l'algoritmo A*
 *
 *  @param map Mappa
 *  @param from Punto di partenza
 *  @param to Punto di arrivo
 *  @param h Funzione per il calcolo della distranza fra 2 punti
 */
void astar(map_t * map, point_t from, point_t to, distance_function h);

#endif
