#include <math.h>

#include "game/ai.h"
#include "game/character.h"
#include "game/game.h"
#include "game/map.h"
#include "game/powerup.h"

#include "main/audio.h"
#include "main/drawing.h"
#include "main/graphics.h"
#include "main/output.h"
#include "main/tiling.h"

#include "pathfinding/pathfinding.h"

#include "misc/random.h"

void character_clear_path(character_t * character)
{

    //  non c'è nessun percorso
    if (!character->path)
        return;

    //  distruzione stack
    stack_delete(character->path);
    character->path = NULL;

    //  la prossima direzione non è nota
    character->next_direction = DIRECTION_NONE;

}

void character_set_location(character_t * character, point_t point, bool clear_direction)
{

    //  il percorso che si sta seguendo perde validità
    character_clear_path(character);

    //  cambiamento di posizione
    character->position = cell_location_to_position(point);
    character->location = point;
    character->last_position = character->position;

    //  azzeramento della direzione
    if (clear_direction)
        character->direction = character->next_direction = DIRECTION_NONE;

}

void character_set_random_position(character_t * character)
{

    map_cell_t * cell;

    do {
        cell = map_random_cell_path(character->map);
    } while (PointEqualToPoint(character->map->end, cell->location));  //  se non coincidone con qualche punto particolare (es. fine mappa)

    character_set_location(character, cell->location, true);

    if (character->is_user)
        character_decide_direction_user(character);
    else
        character_decide_direction_ai(character);
    
}

float character_get_alpha(character_t * character)
{
    
    float * alpha = hashtable_search(character->config, "alpha");
    
    return *alpha ? *alpha : 0.;
    
}

void character_set_alpha(character_t * character, float value)
{

    float * alpha = hashtable_search(character->config, "alpha");

    if (alpha)
        *alpha = value;

}

int character_get_direction_relative_to_location(character_t * character, point_t location)
{
    
    if (location.x < character->location.x) {
        return DIRECTION_WEST;
    } else if (location.x > character->location.x) {
        return DIRECTION_EAST;
    } else if (location.y < character->location.y) {
        return DIRECTION_NORTH;
    } else if (location.y > character->location.y) {
        return DIRECTION_SOUTH;
    }
    
    return DIRECTION_NONE;
    
}

/**
 *  Cambia la direzione di un personaggio
 *
 *  @param character Personaggio del quale cambiare la direzione
 */
sinline void character_set_next_direction(character_t * character)
{
    if (character->next_direction == DIRECTION_NONE)
        return;

    character->direction      = character->next_direction;
    character->next_direction = DIRECTION_NONE;

    character->last_position  = PointNull;
}

/**
 *  Ritorna l'animazione corretta per il movimento che il personaggio sta effettuando
 *
 *  @param character Personaggio del ritornare l'animazione
 *
 *  @return Animazione da utilizzare
 */
animation_t * character_animation(character_t * character)
{

    if (character->direction == DIRECTION_NONE)
        return NULL;

    return character->animations[character->direction];

}

void character_animations_stop(character_t * character)
{

    int i;
    for (i = 0; i < CHARACTER_ANIMATION_LAST; i++) {
        animation_stop(character->animations[i]);
    }

}

void character_set_map(character_t * character, map_t * map)
{
    //  si aggiorna la mappa corrente
    character->map = map;

    //  la nuova posizione del personaggio è la casella di ingresso nella nuova mappa
    character_set_location(character, map->start, true);

}

void character_goto_next_map(game_t * game, character_t * character)
{

    //  prossima mappa
    map_t * next_map = character->map->next;

    //  se dopo la mappa attuale c'è una nuova mappa
    if (next_map) {
        
        if (character->is_user) {

            //  si passa alla prossima mappa
            event_raise(game, EVENT_MAP_NEXT, NULL);
            
        } else {
            
            //  cambio mappa
            character_set_map(character, next_map);

        }
        
    } else {    //  se non c'è una nuova mappa il livello è terminato

        event_raise(game, EVENT_LEVEL_NEXT, character);

    }

}

void character_redraw(game_t * game, character_t * character)
{

    //  se non è il personaggio controllato dall'utente e
    //  si trova in una mappa diversa, non va disegnato nulla
    if (game->user->map != character->map)
        return;

    if (!map_cell_is_valid(character->map, character->location))
        return;

    //  se l'utente ha associato un percorso questo va mostrato sulla mappa
    if (character->is_user && character->path)
        path_draw(character->path, character->path_color);

    //  orientamento del personaggio
    int orientation;

    if (character->direction != DIRECTION_NONE)
        orientation = character->direction;
    else {
        if (character->map->start.x == 0) {
            orientation = DIRECTION_EAST;
        } else {
            orientation = DIRECTION_NORTH;
        }
    }

    //  se l'animazione è attiva verranno ritornati i 3 frame che compongono
    //  l'animazioni di movimento in direzione "character->direction", uno per ogni chiamata ad animation->methods.frame
    animation_t * animation = character->animations[orientation];

    if (animation) {
        output_image_opacity(animation->methods.frame(animation), character->position, character_get_alpha(character) * 255);
    }
}

/**
 *  Controlla se il personaggio è in uscita dai limiti della mappa
 *  e nel caso lo posiziona del punto opposto della mappa
 *
 *  @param character Personaggio da controllare
 */
void character_map_torus(character_t * character)
{

    map_t * map = character->map;

    point_t location = cell_position_to_location(character->position);

    if (!map_cell_is_path(character->map, location))
        return;

    point_t candidate_position = character->position;

    //  la mappa è di tipo toroidale, quindi se si esce da un lato bisogna rientrare dal lato opposto
    if (!PointEqualToPoint(map->start, location) && !PointEqualToPoint(map->end, location)) {

        int width = (int)((map->size.width - 1) * CellSize.width);
        int height = (int)((map->size.height - 1) * CellSize.height);

        bool changed = false;

        //  sconfinamento a est/ovest
        if (candidate_position.x <= 0) {

            //  est
            candidate_position.x = width;
            changed = true;

        } else if (candidate_position.x >= width) {

            //  ovest
            candidate_position.x = 0;
            changed = true;

        } else if (candidate_position.y <= 0) {

            //  sconfinamento a nord
            candidate_position.y = height;
            changed = true;

        } else if (candidate_position.y >= height) {

            //  sconfinamento a sud
            candidate_position.y = 0;
            changed = true;

        }

        //  la posizione è cambiata
        point_t candidate_location = cell_position_to_location(candidate_position);

        if (changed && (!map_cell_is_valid(map, candidate_location) || map_cell_is_path(map, candidate_location))) {

            character_set_location(character, candidate_location, false);
            character_set_alpha(character, 0.);

        }
    }
}

/**
 *  Decrementa il conteggio delle vite di un personaggio, e quando terminano, termina il gioco
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio (utente)
 */
void character_die(game_t * game, character_t * character) {
    
    character->lives--;

    //  vite finite
    if (!character->lives) {
        event_raise(game, EVENT_GAME_LOST, NULL);
    }
    
}

/**
 *  Controlla che tra il personaggio character_a e gli altri personaggi non si siano verificate collisioni
 *
 *  @param game Contesto di gioco
 *  @param character_a Personaggio da controllare
 */
void character_collision_check(game_t * game, character_t * character_a)
{

    //  il personaggio a ignora le collisioni?
    bool ignores_collisions_a = bool_value_nocheck(hashtable_search(character_a->config, "ignores_collisions"));

    //  riquadro nel quale è contenuto il personaggio a
    rectangle_t rect_a = RectMake(character_a->position.x, character_a->position.y, CellSize.width, CellSize.height);

    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    foreach(level->enemies, character_t *, character_b) {

        if (!character_b)
            continue;

        //  i personaggi devono trovarsi nella stessa mappa
        if (character_a->map != character_b->map)
            continue;

        //  e devono essere diversi
        if (character_a == character_b)
            continue;

        //  b ignora le collisioni? (per effetto di un bonus)
        bool ignores_collisions_b = bool_value_nocheck(hashtable_search(character_b->config, "ignores_collisions"));

        //  entrambi ignorano le collisioni, passiamo al prossimo
        if (ignores_collisions_a == ignores_collisions_b && ignores_collisions_a)
            continue;
        
        rectangle_t rect_b = RectMake(character_b->position.x, character_b->position.y, CellSize.width, CellSize.height);

        //  se uno dei personaggi invade la cella dell'altro per più del 60% c'è una collisione
        if (rectangle_intersection_size(rect_a, rect_b) >= 0.6) {

            if (!ignores_collisions_a && character_a->is_user)
                character_die(game, character_a);
            else if (!ignores_collisions_b && character_b->is_user)
                character_die(game, character_b);
            
            //  se il personaggio sta utilizzando un bonus che gli permette di ignorare le collisioni
            //  va spostato solo l'altro personaggio
            if (ignores_collisions_a && !ignores_collisions_b) {

                //  effetto sonoro
                audio_sample_play_by_id(game, AUDIO_SAMPLE_CRASH, false);

                character_set_random_position(character_b);

                character_set_alpha(character_b, 0.);

            } else if (!ignores_collisions_a && ignores_collisions_b) {

                //  effetto sonoro
                audio_sample_play_by_id(game, AUDIO_SAMPLE_CRASH, false);

                character_set_random_position(character_a);

                character_set_alpha(character_a, 0.);

            } else {
                //  i personaggi vengono spostati entrambi in due celle random
                character_set_random_position(character_a);
                character_set_random_position(character_b);

                //  effetto sonoro
                audio_sample_play_by_id(game, AUDIO_SAMPLE_CRASH, false);

                character_set_alpha(character_a, 0.);
                character_set_alpha(character_b, 0.);
            }
        }
    }

}

/**
 *  Calcolo della velocità dei un personaggio in base alla cella in cui si trova
 *  se cell_value = 5 (default) -> velocità = character->speed
 *  se cell_value < 5           -> velocità = character->speed + (5 - cell_value) * 1.5
 *  se cell_value > 5           -> velocità = character->speed / ((cell_value - 5) * 1.5)
 *
 *  @param character Personaggio
 */
float character_get_speed(character_t * character)
{
    
    float base_speed = hashtable_search(character->config, "speed", float);
    int cell_value = cell_get_value(map_get_cell(character->map, character->location));
    
    if (cell_value == CellDefaultValue)
        return base_speed;
    
    if (cell_value < CellDefaultValue)
        return base_speed + (CellDefaultValue - cell_value) * 1.5;
    
    return base_speed / ((cell_value - CellDefaultValue) * 1.5);
    
}

/**
 *  Sposta un personaggio nella prossima posizione a seconda della sua direzione
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio da spostare
 *  @param last_position Ultima posizione del personaggio
 */
void character_move(game_t * game, character_t * character, point_t last_position)
{

    map_t * map = character->map;

    //  la mappa è di tipo toroidale, quindi se si esce da un lato bisogna rientrare dal lato opposto
    character_map_torus(character);

    //  controllo delle collisioni tra personaggi
    character_collision_check(game, character);

    //  aggiornamento cella
    character->location = cell_position_to_location(character->position);

    //  cella nella quale si trova il personaggio
    map_cell_t * cell = map_get_cell(map, character->location);

    //  si controlla se la cella contiene dei bonus
    if (cell->powerup.powerup) {

        //  prova a raccogliere il bonus, se presente
        if (powerup_get(game, character, cell->powerup.powerup)) {

            cell->powerup.powerup = NULL;

        }

    }

    //  avvia l'animazione se c'è stato un movimento
    animation_t * animation = character_animation(character);

    if (animation) {
        if (Manhattan(last_position, character->position) > 0.)
            animation_start(game, animation);
        else
            animation_stop(animation);
    }

    //  è il punto di fine della mappa?
    //  vale a dire il rettangolo che contiene il personaggio
    //  è contenuto completamente nel rettangolo della cella di fine mappa?
    rectangle_t r1 = RectMake(character->position.x, character->position.y, CellSize.width, CellSize.height);
    rectangle_t r2 = RectMake(map->end.x * CellSize.width, map->end.y * CellSize.height, CellSize.width, CellSize.height);

    if (RectEqualToRect(r1, r2)) {
        //  ferma l'animazione
        if (animation)
            animation_stop(animation);

        character_goto_next_map(game, character);
    }

}

/**
 *  Sposta progressivamente un personaggio in una cella
 *
 *  @param character Personaggio
 *  @param dest Destinazione
 */
void character_move_to_cell(character_t * character, point_t dest) {
    
    #define lerp(a, b, t)      (a + ((b - a) * t))

    //  punto di partenza
    point_t src = character->last_position;

    //  % di avanzamento nel percorso tra i due punti
    character->ratio += character_get_speed(character) * TIMER_FREQUENCY;
    character->ratio = RangeNormalizedValue(RangeMake(0., 1.), character->ratio);
    
    //  posizione attuale
    character->position.x = lerp(src.x, dest.x, character->ratio);
    character->position.y = lerp(src.y, dest.y, character->ratio);
    
    #undef lerp
    
}

/**
 *  Se è possibile cambia la direzione del personaggio
 *
 *  @param character Personaggio del quale cambiare la direzione
 */
void character_turn(character_t * character)
{

    //  il personaggio continua ad andare nella stessa direzione
    if (character->next_direction == DIRECTION_NONE)
        return;

    //  inversione di direzione (sempre possibile)
    if (character->next_direction == direction_opposite(character->direction)) {
        character_set_next_direction(character);
        return;
    }

    point_t new_point = character->location;

    //  se il personaggio si sposta in una direzione verso la quale "aumenta" il valore di una qualche coordinata
    //  si deve considerare la cella verso la quale si muove
    if (character->direction == DIRECTION_EAST || character->direction == DIRECTION_SOUTH)  {
        new_point = cell_position_to_location(character->position);
    }

    //  cella sulla quale è diretto il personaggio
    point_t cell_to_check;

    //  in base alla direzione si calcola quale cella controllare (a sinistra, a destra, in alto o in basso)
    switch (character->next_direction) {
        case DIRECTION_EAST:
            cell_to_check = PointPointOffset(new_point, OffsetRight);
            break;
        case DIRECTION_WEST:
            cell_to_check = PointPointOffset(new_point, OffsetLeft);
            break;
        case DIRECTION_NORTH:
            cell_to_check = PointPointOffset(new_point, OffsetTop);
            break;
        case DIRECTION_SOUTH:
            cell_to_check = PointPointOffset(new_point, OffsetBottom);
            break;
    }

    bool horizontal = (character->direction == DIRECTION_WEST || character->direction == DIRECTION_EAST);

    if (( horizontal && character->location.x != floorf(character->position.x / CellSize.width)) ||
        (!horizontal && character->location.y != floorf(character->position.y / CellSize.height))) {

        bool breaks_walls  = bool_value_nocheck(hashtable_search(character->config, "breaks_walls"));

        //  la cella sulla quale il personaggio deve spostarsi è un percorso?
        //  o il personaggio è in grado di abbattere il muro?
        if (map_cell_is_path(character->map, cell_to_check) || (breaks_walls && !map_point_is_on_borders(character->map, cell_to_check))) {

            character_set_next_direction(character);

            character->position = cell_location_to_position(new_point);

            if (character->next_direction == DIRECTION_NORTH)
                character->position.y -= CellSize.height;
            else if (character->next_direction == DIRECTION_SOUTH)
                character->position.y += CellSize.height;
            else if (character->next_direction == DIRECTION_WEST)
                character->position.x -= CellSize.width;
            else if (character->next_direction == DIRECTION_EAST)
                character->position.x += CellSize.width;

        }
    }

}

/**
 *  Ferma l'avanzamento del personaggio quando questo incontra un muro
 *
 *  @param character Personaggio da fermare
 */
void character_wall(game_t * game, character_t * character)
{

    point_t location = character->location;
    point_t point = cell_position_to_location(character->position);

    if (character->direction == DIRECTION_EAST) {
        location.x = point.x++;
    }

    if (character->direction == DIRECTION_SOUTH) {
        location.y = point.y++;
    }

    point_t cell_to_check;

    int direction = character->next_direction != DIRECTION_NONE ? character->next_direction : character->direction;

    switch (direction) {
        case DIRECTION_EAST:
            cell_to_check = PointPointOffset(character->location, OffsetRight);
            break;
        case DIRECTION_WEST:
            cell_to_check = PointPointOffset(character->location, OffsetLeft);
            break;
        case DIRECTION_NORTH:
            cell_to_check = PointPointOffset(character->location, OffsetTop);
            break;
        case DIRECTION_SOUTH:
            cell_to_check = PointPointOffset(character->location, OffsetBottom);
            break;
    }

    bool breaks_walls = bool_value_nocheck(hashtable_search(character->config, "breaks_walls"));

    if (map_cell_is_valid(character->map, point) && !cell_is_path(map_get_cell(character->map, point))) {

        //  il personaggio puà abbattere le mura (purchè non siano quelle che fanno da confine)
        if (breaks_walls && !map_point_is_on_borders(character->map, point)) {

            //  cella rendere un corridoio
            map_cell_t * cell = map_get_cell(character->map, point);

            //  muro -> corridoio
            cell_set_is_path(cell);

            //  effetto sonoro
            audio_sample_play_by_id(game, AUDIO_SAMPLE_WALL, false);

            //  riconnessione delle adiacenze
            map_connect_cell(character->map, cell, 1);

        } else {

            character->position = cell_location_to_position(location);

            if (character->is_user)
                character->last_position = PointNull;

            if (character->next_direction != DIRECTION_NONE &&
               (map_cell_is_path(character->map, cell_to_check) || breaks_walls)) {
                character_set_next_direction(character);
            }

        }
        
    } else if (!map_cell_is_valid(character->map, point)) {

        character->position = map_cell_is_path(character->map, location) ? cell_location_to_position(location) : character->last_position;

        if (character->is_user)
            character->last_position = PointNull;

    }

}

void character_decide_direction_user(character_t * character) {
    
    map_t * map = character->map;
    
    //  si cerca l'unica adiacenza della cella di partenza
    map_cell_t * adjacency[4];
    cell_get_adjacency(map_get_cell(map, map->start), adjacency);
    
    //  la prossima direzione da seguire è quella che porta alla cella adiacenza a quella di partenza
    int direction = character_get_direction_relative_to_location(character, adjacency[0]->location);
    
    //  si setta la direzione
    character->next_direction = direction;
    
}

void character_decide_direction_ai(character_t * character)
{

    map_cell_t * next = stack_head(character->path);

    if (!next)
        return;

    //  la direzione è determinata in base alla posizione del personaggio rispetto al punto next
    character->next_direction = character_get_direction_relative_to_location(character, next->location);

    character_set_next_direction(character);

}

void character_set_path_to(game_t * game, character_t * character, point_t point)
{

    if (PointIsNull(point))
        return;

    //  il personaggio dimentica il vecchio percorso
    character_clear_path(character);

    //  e ne calcola uno nuovo
    ai_path_fiding_function find_path = ai_get_path_function(hashtable_search(character->config, "path_finding_method"));
    find_path(game, character, point);

    //  per poi cominciare a seguirlo
    character_decide_direction_ai(character);

}

void character_update_direction_and_last_position(character_t * character) {
    
    //  controlli sulla direzione
    if (character->next_direction == character->direction)
        character->next_direction = DIRECTION_NONE;
    else if (character->direction == DIRECTION_NONE)
        character->direction = character->next_direction;
    
    if (PointIsNull(character->last_position)) {
        character->last_position = character->position;
        character->ratio = 0.;
    }
    
}

/**
 *  Controlla se nei paraggi c'è qualcosa di interessante (uscita/avversari)
 *  ed eventualmente cambia percorso
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio nei dintorni del quale controllare
 *
 *  @retval true Se il personaggio ha cominciato a seguire un nuovo percorso
 *  @retval false Se non ci sono stati cambiamenti
 */
bool character_rects_check(game_t * game, character_t * character)
{

    dimension_t map_size = SizeMultiplyBySize(character->map->size, CellSize);

    dimension_t exit_search_rect_size = hashtable_search(character->config, "exit_search_rect_size", dimension);
    dimension_t chase_rect_size = hashtable_search(character->config, "chase_rect_size", dimension);

    exit_search_rect_size = SizeMultiplyBySize(exit_search_rect_size, CellSize);
    chase_rect_size = SizeMultiplyBySize(chase_rect_size, CellSize);

    //  il personaggio preferisce inseguire l'utente?
    bool chase_user = bool_value_nocheck(hashtable_search(character->config, "chase_user"));

    //  spazio nel quale cercare l'uscita
    rectangle_t exit_search_rect = rectangle_centered_make(character->position, exit_search_rect_size, map_size);
    //  spazio nel quale cercare avversari (il personaggio dell'utente) da inseguire
    rectangle_t chase_rect = rectangle_centered_make(character->position, chase_rect_size, map_size);

    //  risultato
    bool following = true;

    //  funzione di inseguimento
    ai_chasing_function chase = ai_get_chasing_function(hashtable_search(character->config, "chasing_method"));

    //  utente
    character_t * user = game_get_user(game);

    //  l'avversario da la precendeza all'inseguimento dell'utente
    if (chase_user) {

        if (character->map == user->map && RectContainsPoint(chase_rect, user->position)) {

            //  utente avvistato, nuova destinazione = posizione dell'utente
            if (!chase || !chase(game, character, user))
                character_set_path_to(game, character, user->location);

        } else if (RectContainsPoint(exit_search_rect, cell_location_to_position(character->map->end))) {

            //  se l'utente non è avvistato ma è avvistata l'uscita la si preferisce
            character_set_path_to(game, character, character->map->end);

        } else
            following = false;

    } else {

        //  l'avversario preferisce cercare l'uscita
        //  caso simmetrico

        if (RectContainsPoint(exit_search_rect, cell_location_to_position(character->map->end))) {

            character_set_path_to(game, character, character->map->end);

        } else if (character->map == user->map && RectContainsPoint(chase_rect, user->position)) {

            if (!chase || !chase(game, character, user))
                character_set_path_to(game, character, user->location);

        } else
            following = false;
    }

    return following;

}

/**
 *  Gestione dei movimenti di un personaggo avversario
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio da gestire
 */
void character_control_ai(game_t * game, character_t * character)
{
    
    //  il personaggio non è stato posizionato sulla mappa
    if (PointIsNull(character->location)) {
        return;
    }
    
    //  l'ultimo percorso è stato seguito fino all'ultima cella, si può deallocare
    if (character->path && stack_empty(character->path)) {
        character_clear_path(character);
    }
        
    //  il personaggio non sta ancora seguendo percorsi
    if (!character->path && !character_rects_check(game, character)) {
        
        dimension_t exit_search_rect_size = hashtable_search(character->config, "exit_search_rect_size", dimension);
        exit_search_rect_size = SizeMultiplyBySize(exit_search_rect_size, CellSize);
        
        dimension_t map_size = SizeMultiplyBySize(character->map->size, CellSize);
        
        //  non ci sono nè il personaggio controllato dall'utente nè
        //  l'uscita nei paraggi, si segue il percorso fino alla locazione di tipo
        //  corridoio più vicina all'angolo del rettangolo di ricerca dell'uscita
        //  più vicino all'uscita
        rectangle_t exit_search_rect = rectangle_centered_make(character->position, exit_search_rect_size, map_size);
        
        //  angolo del riquadro exit_search_rect più vicino all'uscita
        point_t closest_point = rectangle_closest_corner_to_point(exit_search_rect, cell_location_to_position(character->map->end));
        point_t candidate_location = cell_position_to_location(closest_point);
        
        //  se il punto è fuori dalla mappa si cerca un punto valido nei paraggi
        while (!map_cell_is_valid(character->map, candidate_location) &&
               !PointEqualToPoint(candidate_location, character->location)) {
            point_t adj[] = {
                PointPointOffset(candidate_location, OffsetTop),
                PointPointOffset(candidate_location, OffsetLeft),
                PointPointOffset(candidate_location, OffsetRight),
                PointPointOffset(candidate_location, OffsetBottom),
            };
            
            candidate_location = adj[random_int(0, sizeof(adj) / sizeof(point_t) - 1)];
        }
        
        //  ricerca di una cella calpestabile nei dintorni del punto candidate_location
        point_t nearest_path = ai_find_nearest_path_location(game, character, candidate_location);
        
        //  calcolo del percorso
        if (!PointIsNull(nearest_path) && !PointEqualToPoint(nearest_path, character->location)) {
            ai_path_fiding_function find_path = ai_get_path_function(hashtable_search(character->config, "path_finding_method"));
            find_path(game, character, nearest_path);
        }
        
        character_decide_direction_ai(character);
        
    } else {

        //  prossima cella da raggiungere
        map_cell_t * next = stack_head(character->path);
        
        if (next) {

            point_t last_position = character->position;
            
            //  controlli sulla direzione
            character_update_direction_and_last_position(character);
            
            //  aggioramento posizione
            character_move_to_cell(character, cell_location_to_position(next->location));
            
            //  controlla se il personaggio si è scontrato con un muro
            character_wall(game, character);
            
            //  spostamento del personaggio
            character_move(game, character, last_position);
            
            //  è stato raggiunto il prossimo punto del percorso, rimozione dallo stack
            if (character->ratio >= 1.) {
                
                if (character->path)
                    stack_pop(character->path);
                
                //  a questo punto si può valure se è il caso di cambiare percorso
                //  ad esempio smettere di cercare l'uscita e inseguire l'avversario
                if (!character_rects_check(game, character)) {
                    //  prossima direzione
                    character_decide_direction_ai(character);
                    
                    //  aggiornamento dell'ultima posizione nota
                    character->last_position = PointNull;
                }
                
            }
            
        }
        
    }
}

/**
 *  Gestione dell'input da testiera e movimenti del personaggio dell'utente
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio da gestire
 */
void character_control_user(game_t * game, character_t * character)
{
    
    point_t last_position = character->position;
    
    //  aggiorna lo stato della tastiera
    keyboard_get_state();
    
    //  controlla la pressione dei tasti direzionali
    //  e aggiorna opportunamente la posizione e la direzione del personaggio
    if (keyboard_is_key_down(KYB_KEY_UP)) {
        
        character->next_direction = DIRECTION_NORTH;
        
    } else if (keyboard_is_key_down(KYB_KEY_DOWN)) {
        
        character->next_direction = DIRECTION_SOUTH;
        
    } else if (keyboard_is_key_down(KYB_KEY_LEFT)) {
        
        character->next_direction = DIRECTION_WEST;
        
    } else if (keyboard_is_key_down(KYB_KEY_RIGHT)) {
        
        character->next_direction = DIRECTION_EAST;
        
    }
    
    //  triggering dei bonus
    foreach(character->powerups, powerup_status_t *, status) {
        
        if (!status->powerup->trigger || status->enabled) continue;
        
        //  se è premuto il tasto che attiva il bonus
        if (keyboard_is_key_down(keyboard_ascii_to_key(status->powerup->trigger))) {
            
            //  mostra, se possibile, il raggio d'azione del bonus
            powerup_area(status->powerup, character);
            
            //  si attiva il bonus
            event_raise(game, EVENT_POWERUP_USE, status);
        }
    }
    
    character_update_direction_and_last_position(character);
    
    point_t next = character->last_position;
    
    if (character->direction == DIRECTION_NORTH)
        next.y -= CellSize.height;
    else if (character->direction == DIRECTION_SOUTH)
        next.y += CellSize.height;
    else if (character->direction == DIRECTION_WEST)
        next.x -= CellSize.width;
    else if (character->direction == DIRECTION_EAST)
        next.x += CellSize.width;
    
    //  aggioramento posizione
    character_move_to_cell(character, next);
    
    character_wall(game, character);

    character_turn(character);
    
    //  se è in corso un movimento si aggiornano le informazioni del personaggio
    //  per il prossimo aggiornamento dello schermo
    character_move(game, character, last_position);

    if (PointEqualToPoint(character->position, next)) {
        character->last_position = PointNull;
    }
    
}

character_t * character_new(bool is_user, image_t * sprite, rectangle_t rect)
{

    character_t * character = memalloc(character_t);

    //  il personaggio è dell'utente?
    character->is_user = is_user;

    //  di default il personaggio è rivolto verso il basso
    character->direction = character->next_direction = DIRECTION_NONE;

    //  posizione sulla mappa, inizialmente una posizione fuori dalla mappa
    character->location = character->position = character->last_position = PointNull;

    //  funzioni per controllare il personaggio (diverse a seconda del tipo utente/ai)
    character->methods.control = is_user ? character_control_user : character_control_ai;

    //  impostazioni di default
    character->ratio = 0.;

    //  si inizializza la lista dei bonus posseduti
    character->powerups = list_new(powerup_status_functions);

    //  in path andrà il percorso da seguire
    character->path = NULL;

    //  colore del percorso da seguire
    character->path_color = ColorRandom(0);

    //  tiling
    character_tiling(character, sprite, rect);

    //  velocità di spostamento del personaggio
    int frames = 3;

    //  inizializzazione delle animazioni per i movimenti del personaggio
    //  ogni animazione è caratterizzata da un set di 3 frames (per questo motivo vanno separate)
    character->animations[CHARACTER_ANIMATION_MOVEMENT_UP]      = animation_new(ANIMATE_FOREVER, frames, character->tiles);
    character->animations[CHARACTER_ANIMATION_MOVEMENT_RIGHT]   = animation_new(ANIMATE_FOREVER, frames, character->tiles + frames);
    character->animations[CHARACTER_ANIMATION_MOVEMENT_DOWN]    = animation_new(ANIMATE_FOREVER, frames, character->tiles + frames * 2);
    character->animations[CHARACTER_ANIMATION_MOVEMENT_LEFT]    = animation_new(ANIMATE_FOREVER, frames, character->tiles + frames * 3);

    return character;

}

void character_delete(character_t * character)
{

    if (!character)
        return;

    //  1. tiles
    int i;
    for (i = 0; i < CHARACTER_TILES_COUNT; i++)
        image_delete(character->tiles[i]);

    //  2. animazioni
    for (i = 0; i < CHARACTER_ANIMATION_LAST; i++) {
        animation_delete(character->animations[i]);
    }

    //  3. eventuali bonus
    list_delete(character->powerups);

    //  4. percorsi
    character_clear_path(character);

    //  5. configurazioni
    hashtable_delete(character->config);

    //  6. configurazioni di default
    hashtable_delete(character->default_config);

    //  7. personaggio
    memfree(character);

}

character_t * character_load(hashtable_t * character_config, bool is_user)
{

    //  path del file contenente le immagini
    char * character_sprites_path = hashtable_search(character_config, "sprites");
    //  zona dell'immagine contenente il personaggio
    rectangle_t * character_rect = hashtable_search(character_config, "rect");

    //  niente personaggi
    if (!character_sprites_path) {
        return NULL;
    }

    //  caricamento textures dei personaggi
    image_t * character_sprites = image_load_new(character_sprites_path, RectZero, true);

    //  probabilmente il percorso dell'immagine è sbagliato
    if (!character_sprites)
        return NULL;

    //  creazione personaggio e inserimento nella lista dei personaggi
    character_t * character = character_new(is_user, character_sprites, *character_rect);

    image_delete(character_sprites);

    if (!character)
        return NULL;

    //  proprietà dei personaggi
    character->config = hashtable_duplicate(character_config);

    //  controlla che nella configurazione ci siano tutti i valori, altrimenti inserisce quelli di default

    //  velocità
    if (!hashtable_search(character->config, "speed")) {
        float speed = 3.5;
        hashtable_insert(character->config, "speed", &speed, float_functions, true);
    }

    //  opacità
    if (!hashtable_search(character->config, "alpha")) {
        float alpha = 1.;
        hashtable_insert(character->config, "alpha", &alpha, float_functions, true);
    }

    //  possibilità di abbattere le mura
    if (!hashtable_search(character->config, "breaks_walls")) {
        long breaks_walls = 0;
        hashtable_insert(character->config, "breaks_walls", &breaks_walls, long_functions, true);
    }

    //  possibilità di ignorare le collisioni
    if (!hashtable_search(character->config, "ignores_collisions")) {
        long ignores_collisions = 0;
        hashtable_insert(character->config, "ignores_collisions", &ignores_collisions, long_functions, true);
    }

    if (!character->is_user) {

        //  riquadro di inserimento
        dimension_t chase_rect_size = dimension_value(hashtable_search(character->config, "chase_rect_size"));

        if (SizeIsZero(chase_rect_size))
            chase_rect_size = SizeMake(2., 2.);

        if (!hashtable_replace(character->config, "chase_rect_size", &chase_rect_size, true))
            hashtable_insert(character->config, "chase_rect_size", &chase_rect_size, dimension_functions, true);

        //  riquadro di uscita
        dimension_t exit_rect_size = dimension_value(hashtable_search(character->config, "exit_search_rect_size"));

        if (SizeIsZero(exit_rect_size))
            exit_rect_size = SizeMake(2., 2.);

        if (!hashtable_replace(character->config, "exit_search_rect_size", &exit_rect_size, true))
            hashtable_insert(character->config, "exit_search_rect_size", &exit_rect_size, dimension_functions, true);

        //  preferenza di comportamento
        if (!hashtable_search(character->config, "chase_user")) {
            long value = 1;
            hashtable_insert(character->config, "chase_user", &value, long_functions, true);
        }

    } else {
        
        //  area visibile all'utente
        dimension_t visible_area_size = dimension_value(hashtable_search(character->config, "visible_area_size"));
        
        if (SizeIsZero(visible_area_size))
            visible_area_size = SizeInfinity;
        
        if (!hashtable_replace(character->config, "visible_area_size", &visible_area_size, true))
            hashtable_insert(character->config, "visible_area_size", &visible_area_size, dimension_functions, true);
        
        //  vite
        long * lives_current = (long *)hashtable_search(character->config, "lives");
        long lives = 5;
        
        if (lives_current && 0 <= *lives_current && *lives_current <= UCHAR_MAX) {
            lives = *lives_current;
        }
        
        character->lives = lives;
        
    }

    //  proprietà di default
    character->default_config = hashtable_duplicate(character->config);

    debugf("[Personaggio] %s caricato\n", is_user ? "dell'utente" : "avversario");

    return character;

}

//  Carica tutti i personaggi specificati nella variabile di tipo lista "name" del file di configurazione del livello
list_t * characters_load(hashtable_t * config, char * name)
{

    //  lista dei nomi dei personaggi
    list_t * characters_names = hashtable_search(config, name);

    //  lista dei personaggi
    list_t * characters = list_new(character_functions);

    foreach(characters_names, char *, character_name) {

        //  la configurazione è contenuta in un campo di tipo dictionary di nome "name"
        hashtable_t * character_config = hashtable_search(config, character_name);

        //  inizializzazione del personaggio
        character_t * character = character_load(character_config, 0);

        if (character)
            list_insert(characters, character);

    }

    return characters;

}

/**
 *  Handler del timer che controlla la velocità delle animazioni dei personaggi
 */
void character_handle_timer(game_t * game, event_t evt)
{

    if (!TimerEqualToTimer(game->characters_timer, evt.object))
        return;

    if (!game_is_started(game))
        return;

    character_t * user = game_get_user(game);

    //  trasparenza
    float alpha = character_get_alpha(user);
    
    //  aggiornamento trasparenza
    if (alpha < 1.)
        character_set_alpha(user, alpha + 0.1);

    //  animazione del personaggio dell'utente
    animation_t * animation = character_animation(user);

    if (animation) {
        animation->methods.animate(game, animation);
    }
    
    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    //  animazione dei personaggi avversari
    foreach(level->enemies, character_t *, character) {

        //  trasparenza
        alpha = character_get_alpha(character);
        
        //  aggiornamento
        if (alpha < 1.)
            character_set_alpha(character, alpha + 0.1);

        if ((animation = character_animation(character))) {
            animation->methods.animate(game, animation);
        }
        
    }

}

void characters_register_event_handlers(game_t * game)
{

    event_subscribe(game, EVENT_TIMER_TICK, character_handle_timer);

}

TYPE_FUNCTIONS_DEFINE(character, character_delete);
