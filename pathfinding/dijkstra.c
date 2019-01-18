#include "game/map.h"
#include "game/cell.h"
#include "game/structs.h"

#include "pathfinding/pathfinding.h"

#include "std/priority_queue.h"

void dijkstra(map_t * map, point_t u)
{

    //  inizializzazione del colore e del predecessore
    map_clear_graph(map);

    //  distanza della cella di partenza
    cell_set_distance(map_get_cell(map, u), 0);

    int cells = map->size.width * map->size.height;
    priority_queue_t * pqueue = priority_queue_new(PRIORITY_QUEUE_MIN, cells);
    
    priority_queue_insert(pqueue, 0, map_get_cell(map, u));

    while (!priority_queue_empty(pqueue)) {

        map_cell_t * cell = (map_cell_t *)priority_queue_extract_min(pqueue);
        map_cell_t * adjacency[4];
        int length = cell_get_adjacency(cell, adjacency);

        int i;
        for (i = 0; i < length; i++) {

            map_cell_t * n = adjacency[i];

            if (cell_is_path(n)) {

                float distance = cell_get_distance(cell) + cell_get_value(n);

                if (cell_get_distance(n) > distance) {
                    cell_set_parent(n, cell);
                    cell_set_distance(n, distance);

                    if (cell_get_color(n) == CELL_COLOR_WHITE) {
                        cell_set_color(n, CELL_COLOR_GRAY);
                        priority_queue_insert(pqueue, distance, n);
                    }
                }
            }

        }

        cell_set_color(cell, CELL_COLOR_BLACK);

    }

    priority_queue_delete(pqueue);

}