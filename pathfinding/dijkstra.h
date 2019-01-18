#ifndef pathfinding_dijkstra_h
#define pathfinding_dijkstra_h

#include "game/structs.h"

/**
 *  Esplora il grafo di una mappa utilizzando l'algoritmo di Dijkstra
 *
 *  @param map Mappa
 *  @param u Punto di partenza
 */
void dijkstra(map_t * map, point_t u);

#endif
