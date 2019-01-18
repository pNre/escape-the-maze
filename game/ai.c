#include "utils.h"

#include "game/ai.h"
#include "game/game.h"
#include "game/character.h"
#include "game/map.h"

#include "std/queue.h"

#include "pathfinding/pathfinding.h"

#include "misc/random.h"

void ai_find_path_bfs(game_t * game, character_t * character, point_t to)
{

    //  creazione di un nuovo stack contenente i nodi del percorso
    if (!character->path)
        character->path = stack_new();

    //  algoritmo bfs
    bfs(character->map, character->location);

    //  popolamento dello stack con i nodi del percorso
    shortest_path_find(character->map, map_get_cell(character->map, character->location), map_get_cell(character->map, to), character->path);

}

void ai_find_path_dijkstra(game_t * game, character_t * character, point_t to)
{

    //  creazione di un nuovo stack contenente i nodi del percorso
    if (!character->path)
        character->path = stack_new();

    //  algoritmo di dijkstra
    dijkstra(character->map, character->location);

    //  popolamento dello stack con i nodi del percorso
    shortest_path_find(character->map, map_get_cell(character->map, character->location), map_get_cell(character->map, to), character->path);

}

void ai_find_path_astar(game_t * game, character_t * character, point_t to)
{

    //  creazione di un nuovo stack contenente i nodi del percorso
    if (!character->path)
        character->path = stack_new();

    //  algoritmo A*
    astar(character->map, character->location, to, Manhattan);

    //  popolamento dello stack con i nodi del percorso
    shortest_path_find(character->map, map_get_cell(character->map, character->location), map_get_cell(character->map, to), character->path);

}

bool ai_chase_predict_position(game_t * game, character_t * chaser, character_t * character)
{

    if (character->direction == DIRECTION_NONE)
        return false;

    if (chaser->map != character->map)
        return false;

    map_cell_t * farthest_cell = map_get_cell(character->map, character->location);
    point_t offset = PointNull;

    switch (character->direction) {
        case DIRECTION_NORTH:
            offset = OffsetTop;
            break;
        case DIRECTION_SOUTH:
            offset = OffsetBottom;
            break;
        case DIRECTION_WEST:
            offset = OffsetLeft;
            break;
        case DIRECTION_EAST:
            offset = OffsetRight;
            break;
        default:
            break;
    }

    int max_distance = random_int(4, 8);
    
    //  cerca di prevedere la cella più lontana che il personaggio può raggiungere tenendo la sua direzione attuale
    while (map_cell_is_path(character->map, PointPointOffset(farthest_cell->location, offset))) {
        
        farthest_cell = map_get_cell(character->map, PointPointOffset(farthest_cell->location, offset));
        
        if (--max_distance == 0)
            break;
        
    }

    //  se la cella da raggiungere è sullo stesso asse della cella dell'inseguitore
    //  oppure l'inseguitore è vicino all'inseguito, si raggiunge la cella del personaggio inseguito
    if (farthest_cell->location.x == chaser->location.x ||
        farthest_cell->location.y == chaser->location.y ||
        Manhattan(farthest_cell->location, character->location) <= 1.) {

        farthest_cell = map_get_cell(character->map, character->location);

    }

    character_set_path_to(game, chaser, farthest_cell->location);

    return true;

}

bool ai_chase_trap(game_t * game, character_t * chaser, character_t * character)
{

    character_t * third = NULL;
    
    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    //  cerca un terzo personaggio nella stessa mappa con il quale "fare l'imboscata"
    foreach(level->enemies, character_t *, enemy) {
        if (enemy != chaser && enemy->map == chaser->map) {
            third = enemy;
            break;
        }
    }

    if (!third)
        return false;

    //  a seconda della direzione ci sono 3 punti utilizzabile
    point_t offsets[3];

    if (character->direction == DIRECTION_NORTH) {
        offsets[0] = PointMultiply(OffsetTopLeft, 2);
        offsets[1] = PointMultiply(OffsetTopRight, 2);
        offsets[2] = PointMultiply(OffsetTop, 2);
    }
    else if (character->direction == DIRECTION_SOUTH) {
        offsets[0] = PointMultiply(OffsetBottomLeft, 2);
        offsets[1] = PointMultiply(OffsetBottomRight, 2);
        offsets[2] = PointMultiply(OffsetBottom, 2);
    }
    else if (character->direction == DIRECTION_EAST) {
        offsets[0] = PointMultiply(OffsetRight, 2);
        offsets[1] = PointMultiply(OffsetBottomRight, 2);
        offsets[2] = PointMultiply(OffsetTopRight, 2);
    }
    else if (character->direction == DIRECTION_WEST) {
        offsets[0] = PointMultiply(OffsetLeft, 2);
        offsets[1] = PointMultiply(OffsetBottomLeft, 2);
        offsets[2] = PointMultiply(OffsetTopLeft, 2);
    }
    else
        return false;
 
    //  si sceglie uno dei 3 punti in modo casuale
    point_t point = PointPointOffset(character->location, offsets[random_int(0, 2)]);
    
    //  il punto è fuori dalla mappa, utilizziamo un'altra strategia
    if (!map_cell_is_valid(character->map, point))
        return false;

    //  punto da raggiungere
    point_t destination = PointZero;

    destination.x = 2 * point.x - third->location.x;
    destination.y = 2 * point.y - third->location.y;

    //  fuori dalla mappa
    if (!map_cell_is_valid(character->map, destination))
        return false;
    
    int distance = 1;
    
    //  se il punto non è un percorso, si cerca un percorso nei paraggi
    while (!map_cell_is_path(character->map, destination)) {
        
        point_t offs[4] = {
            OffsetTop,
            OffsetLeft,
            OffsetRight,
            OffsetBottom
        };
        
        destination = PointMultiply(offs[random_int(0, (sizeof(offs) / sizeof(point_t) - 1))], distance);
        distance++;
        
    }

    character_set_path_to(game, chaser, destination);

    return true;
}

//  Cerca la cella calpestabile più vicina a source
point_t ai_find_nearest_path_location(game_t * game, character_t * character, point_t source)
{

    map_cell_t * cell = map_get_cell(character->map, source);

    //  se la cella da raggiungere non è un corridoio
    if (!cell_is_path(cell)) {

        //  si cerca un corridoio nei dintorni della cella
        list_t * Q = list_new(no_functions);
        list_insert(Q, cell);

        //  finchè ci sono nodi
        while (!list_empty(Q)) {

            //  estrazione di un elemento casuale dalla lista
            cell = list_pop_random(Q);

            if (cell_is_path(cell))
                break;
            
            //  se la cella non è calpestabile
            //  si accodano tutte le celle nei paraggi
            int x, y;
            for (y = -1; y <= 1; y++) {
                for (x = -1; x <= 1; x++) {

                    //  x == y && y == 0 è proprio la cella cell
                    if (x == y && y == 0) continue;

                    point_t point = PointOffset(cell->location, x, y);

                    //  non è una cella valida (es. è fuori dai limiti della mappa)
                    if (!map_cell_is_valid(character->map, point))
                        continue;

                    map_cell_t * neighbor = map_get_cell(character->map, point);

                    if (cell_is_path(neighbor))
                        list_insert(Q, neighbor);

                }
            }

        }

        list_delete(Q);

        //  cella trovata
        if (cell)
            return cell->location;

    }

    return cell->location;

}
