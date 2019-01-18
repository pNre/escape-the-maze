#include "game/map.h"
#include "game/cell.h"
#include "game/structs.h"

#include "pathfinding/pathfinding.h"

#include "std/priority_queue.h"

void astar(map_t * map, point_t from, point_t to, distance_function h)
{

    map_clear_graph(map);

    map_cell_t * start = map_get_cell(map, from);
    map_cell_t * end = map_get_cell(map, to);

    dimension_t map_size = map->size;

    const int size = map_size.width * map_size.height;

    int * g = memalloc(int, size, 1);
    
    //  nodi inseriti nella coda a priorità, indicizzati in base alle coordinate
    //  in questo modo è possibile eseguire il decremento delle chiavi senza dover cercare il nodo
    priority_queue_node_t ** pq_nodes = memalloc(priority_queue_node_t *, size, 1);

    priority_queue_t * open_set = priority_queue_new(PRIORITY_QUEUE_MIN, size);
    priority_queue_insert(open_set, h(from, to), start);

    cell_set_color(start, CELL_COLOR_GRAY);

    int found = 0;

    while (!priority_queue_empty(open_set) || !found) {

        map_cell_t * cell = (map_cell_t *)priority_queue_extract_min(open_set);

        if (!cell)
            break;

        //  indice del nodo della coda a priorità nell'array pq_nodes
        int node_index = map_cell_location_to_index(map, cell->location);
        
        //  il nodo è stato estratto quindi eliminato dalla coda
        pq_nodes[node_index] = NULL;

        //  trovato il nodo destinazione
        if (PointEqualToPoint(cell->location, end->location))
            found = 1;
        else {
            cell_set_color(cell, CELL_COLOR_BLACK);

            map_cell_t * adjacency[4];
            int length = cell_get_adjacency(cell, adjacency);

            int i;
            for (i = 0; i < length; i++) {

                map_cell_t * n = adjacency[i];

                //  se la cella è sui confini della mappa qualche adiacente potrebbe essere NULL
                if (!cell_is_path(n)) continue;

                //  calcolo del costo tenendo in considerazione il valore (peso) della cella
                int cost = g[node_index] + cell_get_value(n);
                
                //  indice del nodo della coda a priorità nell'array pq_nodes
                int adj_index = map_cell_location_to_index(map, n->location);
                
                if (cell_get_color(n) == CELL_COLOR_WHITE || cost < g[adj_index]) {

                    cell_set_parent(n, cell);

                    g[adj_index] = cost;

                    if (cell_get_color(n) != CELL_COLOR_GRAY) {
                        pq_nodes[adj_index] = priority_queue_insert(open_set, cost + h(cell->location, n->location), n);
                        cell_set_color(n, CELL_COLOR_GRAY);
                    } else {
                        if (pq_nodes[adj_index])
                            priority_queue_decrease_key(open_set, pq_nodes[adj_index], cost + h(cell->location, n->location));
                    }

                }
            }
        }

    }

    priority_queue_delete(open_set);

    memfree(g);
    memfree(pq_nodes);

}
