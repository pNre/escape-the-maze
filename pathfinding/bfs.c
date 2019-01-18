#include "game/map.h"
#include "game/cell.h"
#include "game/structs.h"

#include "pathfinding/pathfinding.h"

#include "std/queue.h"

void bfs(map_t * map, point_t u)
{

    //  inizializzazione del colore e del predecessore
    map_clear_graph(map);

    //  coda per la bfs
    queue_t * Q = queue_new();
    
    queue_push(Q, map_get_cell(map, u));

    while (!queue_empty(Q)) {

        map_cell_t * cell = (map_cell_t *)queue_pop(Q);

        map_cell_t * adjacency[4];
        int length = cell_get_adjacency(cell, adjacency);

        int i;
        for (i = 0; i < length; i++) {

            map_cell_t * n = adjacency[i];

            //  se è un percorso e non è stato visitato
            if (cell_is_path(n) && cell_get_color(n) == CELL_COLOR_WHITE) {
                cell_set_color(n, CELL_COLOR_GRAY);
                cell_set_parent(n, cell);
                queue_push(Q, n);
            }

        }

        cell_set_color(cell, CELL_COLOR_BLACK);

    }

    queue_delete(Q);

}



