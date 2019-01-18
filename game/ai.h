#ifndef game_ai_h
#define game_ai_h

#include "types.h"

#include "game/structs.h"

#include "misc/geometry.h"

typedef void (* ai_path_fiding_function)(game_t *, character_t *, point_t);
typedef bool (* ai_chasing_function)(game_t *, character_t *, character_t *);

/**
 *  Mappa le funzioni di ricerca di un percorso attraverso il loro nome
 */
typedef struct ai_path_fiding_functions_table_entry_s {

    /** Nome, univoco, della funzione */
    char * name;

    /** Puntatore alla funzione */
    ai_path_fiding_function function;

} ai_path_fiding_functions_table_entry;

/**
 *  Mappa le funzioni di inseguimento attraverso il loro nome
 */
typedef struct ai_chasing_functions_table_entry_s {

    /** Nome, univoco, della funzione */
    char * name;

    /** Puntatore alla funzione */
    ai_chasing_function function;

} ai_chasing_functions_table_entry;

/**
 *  Calcolo del percorso minimo dalla locazione corrente del personaggio _character_
 *  al punto della mappa _to_ utilizzando l'algoritmo BFS
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio rispetto al quale effettuare il calcolo
 *  @param to Punto di destinazione
 */
void ai_find_path_bfs(game_t * game, character_t * character, point_t to);

/**
 *  Calcolo del percorso minimo dalla locazione corrente del personaggio _character_
 *  al punto della mappa _to_ utilizzando l'algoritmo di Dijkstra
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio rispetto al quale effettuare il calcolo
 *  @param to Punto di destinazione
 */
void ai_find_path_dijkstra(game_t * game, character_t * character, point_t to);

/**
 *  Calcolo del percorso minimo dalla locazione corrente del personaggio _character_
 *  al punto della mappa _to_ utilizzando l'algoritmo A*
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio rispetto al quale effettuare il calcolo
 *  @param to Punto di destinazione
 */
void ai_find_path_astar(game_t * game, character_t * character, point_t to);

/**
 *  Ricerca intorno ad un punto _source_ una cella di tipo corridoio
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio rispetto al quale effettuare il calcolo
 *  @param source Punto intorno al quale effettuare la ricerca
 *
 *  @return Cella trovata
 *  @retval PointNull se non è stata trovata nessuna cella
 */
point_t ai_find_nearest_path_location(game_t * game, character_t * character, point_t source);

/**
 *  Strategia di inseguimento di un personaggio basata sulla predizione dei movimenti
 *
 *  @param game Contesto di gioco
 *  @param chaser Personaggio inseguitore
 *  @param character Personaggio da inseguire
 *
 *  @return Stato dell'inseguimento
 *  @retval true Inseguimento avviato
 */
bool ai_chase_predict_position(game_t * game, character_t * chaser, character_t * character);

/**
 *  Strategia di inseguimento di un personaggio basata sul tentativo di bloccare il personaggio da due lati.
 *  Il calcolo del punto da raggiungere è basato sulla posizione di uno degli altri avversari e su quella del personaggio dell'utente.
 *
 *  @param game Contesto di gioco
 *  @param chaser Personaggio inseguitore
 *  @param character Personaggio da inseguire
 *
 *  @return Stato dell'inseguimento
 *  @retval true Inseguimento avviato
 */
bool ai_chase_trap(game_t * game, character_t * chaser, character_t * character);

/**
 *  Dato il nome di una funzione per il calcolo del percorso minimo nè ritorna il puntatore
 *
 *  @param name Nome dealla funzione
 *
 *  @return Puntatore alla funzione
 */
sinline ai_path_fiding_function ai_get_path_function(char * name)
{

    if (!name)
        return ai_find_path_bfs;

    //  mappa delle funzioni supportate
    //  il nome è lo stesso che può essere utilizzato nei files di configurazione
    const ai_path_fiding_functions_table_entry ai_path_fiding_functions[] = {
        { "bfs",        ai_find_path_bfs },
        { "dijkstra",   ai_find_path_dijkstra },
        { "a*",         ai_find_path_astar },
    };

    unsigned int i;

    //  loop sui contenuti della tabella
    for (i = 0; i < array_count(ai_path_fiding_functions); i++) {
        //  quando i nomi coincidono si ritorna la funzione
        if (!strcasecmp(ai_path_fiding_functions[i].name, name))
            return ai_path_fiding_functions[i].function;
    }

    return ai_find_path_bfs;

}

/**
 *  Dato il nome di una funzione per l'inseguimento nè ritorna il puntatore
 *
 *  @param name Nome dealla funzione
 *
 *  @return Puntatore alla funzione
 */
sinline ai_chasing_function ai_get_chasing_function(char * name)
{

    if (!name)
        return NULL;

    //  mappa delle funzioni supportate
    //  il nome è lo stesso che può essere utilizzato nei files di configurazione
    const ai_chasing_functions_table_entry ai_chasing_functions[] = {
        { "predict position",   ai_chase_predict_position },
        { "trap",               ai_chase_trap }
    };

    unsigned int i;

    //  loop sui contenuti della tabella
    for (i = 0; i < array_count(ai_chasing_functions); i++) {
        //  quando i nomi coincidono si ritorna la funzione
        if (!strcasecmp(ai_chasing_functions[i].name, name))
            return ai_chasing_functions[i].function;
    }

    return NULL;

}

#endif
