#ifndef game_character_h
#define game_character_h

#include "std/hashtable.h"
#include "std/stack.h"

#include "misc/geometry.h"
#include "misc/directions.h"

#include "main/image.h"
#include "game/animations.h"

#include "game/ai.h"
#include "game/events.h"
#include "game/structs.h"

#define CHARACTER_TILES_COUNT 12

/**
 *  Definisce i tipi di animazione che un personaggio può avere
 */
enum {

    CHARACTER_ANIMATION_MOVEMENT_UP = DIRECTION_NORTH,
    CHARACTER_ANIMATION_MOVEMENT_RIGHT = DIRECTION_EAST,
    CHARACTER_ANIMATION_MOVEMENT_DOWN = DIRECTION_SOUTH,
    CHARACTER_ANIMATION_MOVEMENT_LEFT = DIRECTION_WEST,

    CHARACTER_ANIMATION_LAST

};

/**
 *  Contiene tutte le informazioni relative ad un personaggio
 */
struct character_s {

    /** Specifica se il personaggio è controllato dall'utente */
    bool is_user;

    /** Bitmap del personaggio (memorizzate a seconda dei movimenti) */
    image_t * tiles[CHARACTER_TILES_COUNT];

    /** Posizione nel corso del movimento tra due celle (0., 1.) */
    float ratio;

    /** Animazioni che consentono l'alternarsi dei frame che simulano il movimento del personaggio */
    animation_t * animations[CHARACTER_ANIMATION_LAST];

    /** Posizione del personaggio sulla mappa in relazione alla griglia */
    point_t location;

    /** Posizione assoluta del personaggio sulla mappa in relazione allo schermo */
    point_t position;

    /** Ultima posizione assoluta del personaggio */
    point_t last_position;

    /** Mappa nella quale si trova il personaggio */
    map_t * map;

    /** Direzione che il personaggio sta tenendo */
    int direction;

    /** Direzione verso la quale il personaggio va girato (se possibile) */
    int next_direction;

    /** Percorso da seguire (se è un avversario) o percorso da mostrare sulla mappa (se è l'utente) */
    sstack_t * path;

    /** Colore con il quale il percorso _path_ è disegnato sulla mappa */
    color_t path_color;

    /** Lista dei bonus in possesso dell'utente */
    list_t * powerups;
    
    /** Vite del personaggio 0-255 */
    unsigned char lives;

    /** Raccoglie i metodi per la gestione dei personaggi */
    struct {

        /** Puntatore alla funzione che si occupa di determinare quali saranno i movimenti del personaggio */
        void (* control)(game_t *, struct character_s *);

    } methods;

    /** Proprietà attuali del personaggio */
    hashtable_t * config;

    /** Proprietà di default del personaggio */
    hashtable_t * default_config;

};

/**
 *  Caricamento dei personaggi definiti nella variabile lista _name_ del file di configurazione _config_
 *
 *  @param config Hashtable contenente la configurazione dei personaggi
 *  @param name Nome della variabile di tipo lista che contiene i nomi delle variabili che definiscono i personaggi
 *
 *  @return Lista dei personaggi (character_t *) caricati
 *
 *  @retval NULL In caso di errori
 */
list_t * characters_load(hashtable_t * config, char * name);

/**
 *  Caricamento del personaggio definito nella hashtable _character_config_
 *
 *  @param character_config Hashtable contenente la configurazione del personaggio
 *  @param is_user Indica se il personaggio è controllato dall'utente
 *
 *  @return Personaggio caricato
 *
 *  @retval NULL In caso di errori
 */
character_t * character_load(hashtable_t * character_config, bool is_user);

/**
 *  Creazione di un nuovo personaggio
 *
 *  @param is_user Indica se il personaggio è controllato dall'utente
 *  @param texture Bitmap del personaggio
 *  @param rect Area della bitmap dedicata a questo personaggio
 *
 *  @return Personaggio
 */
character_t * character_new(bool is_user, image_t * texture, rectangle_t rect);

/**
 *  Deallocazione di un personaggio
 *
 *  @param character Personaggio da deallocare
 */
void character_delete(character_t * character);

/**
 *  Ridisegna il personaggio sullo schermo
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio da ridisegnare
 */
void character_redraw(game_t * game, character_t * character);

/**
 *  Ferma tutte le animazioni di un personaggio
 *
 *  @param character Personaggio
 */
void character_animations_stop(character_t * character);

/**
 *  Porta un personaggio alla mappa successiva
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio che deve cambiare mappa
 */
void character_goto_next_map(game_t * game, character_t * character);

/**
 *  Svuota il campo _path_ di un personaggio
 *
 *  @param character Personaggio del quale svuotare il percorso
 */
void character_clear_path(character_t * character);

/**
 *  Impostazione della mappa per un personaggio
 *
 *  @param character Personaggio
 *  @param map Mappa
 *  @param first Indica se si tratta della prima mappa
 */
void character_set_map(character_t * character, map_t * map);

/**
 *  Rettifica la posizione dell'utente al cambio mappa
 *
 *  @param character Personaggio
 */
void character_decide_direction_user(character_t * character);

/**
 *  Valuta se è il caso (in base al prossimo punto del percorso da raggiungere)
 *  di cambiare la direzione che il personaggio sta seguendo
 *
 *  @param character Personaggio sul quale eseguire il controllo
 */
void character_decide_direction_ai(character_t * character);

/**
 *  Determina la direzione in cui dirigere un personaggio per raggiungere una determinata cella
 *
 *  @param character Personaggio
 *  @param location Coordinate della cella da raggiungere
 *
 *  @return Direzione
 */
int character_direction_relative_to_location(character_t * character, point_t location);

/**
 *  Cambia la posizione di un personaggio
 *
 *  @param character Personaggio del quale cambiare la posizione
 *  @param point Nuova posizione (in termini di celle)
 *  @param clear_direction Se vero il personaggio sarà fero dopo il cambio di posizione
 */
void character_set_location(character_t * character, point_t point, bool clear_direction);

/**
 *  Sposta due personaggi in due celle random ad una certa distanza l'una dall'altra
 *
 *  @param character_a Personaggio del quale cambiare la posizione
 *  @param character_b Personaggio del quale cambiare la posizione
 *  @param min_distance Distanza minima tra le due posizioni
 */
void characters_set_random_position(character_t * character_a, character_t * character_b, int min_distance);

/**
 *  Sposta un personaggio in una cella random
 *
 *  @param character Personaggio del quale cambiare la posizione
 */
void character_set_random_position(character_t * character);

/**
 *  Calcola il percorso fino ad un punto e lo associa al personaggio
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio
 *  @param point Destinazione
 */
void character_set_path_to(game_t * game, character_t * character, point_t point);

/**
 *  Registra gli handler degli eventi
 *
 *  @param game Contesto di gioco
 */
void characters_register_event_handlers(game_t * game);

#endif
