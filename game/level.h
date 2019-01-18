#ifndef game_level_h
#define game_level_h

#include "std/list.h"
#include "std/hashtable.h"

#include "game/cell.h"
#include "game/structs.h"

#include "main/image.h"
#include "game/animations.h"

/**
 *  Definisce le animazioni possibili per un livello
 */
enum {
    LEVEL_ANIMATION_MAP_TRANSITION_OUT,
    LEVEL_ANIMATION_MAP_TRANSITION_IN,

    LEVEL_ANIMATION_LAST
};

/**
 *  Definisce le texture utilizzate dal livello
 */

enum {
    LEVEL_TEXTURE_UNKNOWN,
    LEVEL_TEXTURE_WALL,

    LEVEL_TEXTURE_PATH_LIGHTEST,
    LEVEL_TEXTURE_PATH_DARKEST = LEVEL_TEXTURE_PATH_LIGHTEST + 8,

    LEVEL_TEXTURE_LAST
};

/**
 *  Contiene tutte le informazioni su un livello
 */
struct level_s {

    /** Nome del livello */
    char * name;

    /** Mappa che costituiscono il livello, ordinate */
    list_t * maps;

    /**
     *  Textures delle celle livello:
     *  - Corridoi
     *  - Mura
     */
    image_t * textures[LEVEL_TEXTURE_LAST];

    /** Animazioni del livello */
    animation_t * animations[LEVEL_ANIMATION_LAST];

    /** Avversari */
    list_t * enemies;

    /**
     *  Livello di complessità da 0. a 1., utilizzato nella generazione delle mappe random,
     *  più è alto più le mappe saranno complicate
     */
    float complexity;

};

/**
 *  Caricamento dei livelli utilizzando le informazioni contenute nel file di configurazione 
 *
 *  @param game Contesto di gioco
 *  @param config Hashtable contenente la configurazione
 *
 *  @return Lista dei livelli caricati
 *
 *  @retval NULL In caso di errori
 */
queue_t * levels_load(game_t * game, hashtable_t * config);

/**
 *  Inizializzazione di un livello, necessaria nel passaggio da un livello all'altro
 *
 *  @param game Contesto di gioco
 *  @param level Livello al quale passare
 */
void level_setup(game_t * game, level_t * level);

/**
 *  De-inizializzazione di un livello, necessaria nel passaggio da un livello all'altro
 *
 *  @param game Contesto di gioco
 *  @param level Livello dal quale si proviene
 */
void level_uninstall(game_t * game, level_t * level);

/**
 *  Deallocazione di un livello
 *
 *  @param level Livello al quale passare
 */
void level_delete(level_t * level);

/**
 *  Creazione di una lista contenente i personaggi del livello attuale.
 *  Lista = Avversari + {Utente}
 *
 *  @param game Contesto di gioco
 *
 *  @return Lista dei personaggi
 */
list_t * level_get_characters_list(game_t * game);

/**
 *  Registra gli handler degli eventi
 *
 *  @param game Contesto di gioco
 */
void levels_register_event_handlers(game_t * game);

#endif
