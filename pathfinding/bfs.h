#ifndef pathfinding_bfs_h
#define pathfinding_bfs_h

#include "game/structs.h"

/**
 *  Esplora il grafo di una mappa utilizzando l'algoritmo BFS
 *
 *  @param map Mappa
 *  @param u Punto di partenza
 */
void bfs(map_t * map, point_t u);

#endif
