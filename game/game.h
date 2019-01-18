#ifndef game_game_h
#define game_game_h

#include "std/list.h"
#include "std/hashtable.h"

#include "game/character.h"
#include "game/level.h"
#include "game/structs.h"

#include "main/timer.h"
#include "game/animations.h"

/**
 *  Animazioni globali
 */
enum {
    GAME_ANIMATION_LEVEL_NEXT,
    
    GAME_ANIMATION_OVER,
    GAME_ANIMATION_WON,

    GAME_ANIMATION_LAST
};

/**
 *  Definisce gli stati del gioco
 */
enum {

    /** Gioco avviato */
    GAME_STATE_STARTED = 1 << 0,

    /** Gioco in corso */
    GAME_STATE_RUNNING = 1 << 1,

    /** Gioco terminato */
    GAME_STATE_OVER = 1 << 2,

    /** Vittoria */
    GAME_STATE_WON = 1 << 3

};

/** ID delle animazioni */
#define LEVEL_NEXT_FADING_OUT_ANIMATION 0xEEE00EEE
#define LEVEL_NEXT_TRANSITION_ANIMATION 0xEEE11EEE

/**
 *  Contiene tutte le informazioni sullo stato del gioco
 */
struct game_s {
    
    /** Stato del gioco: in pausa/in gioco */
    int state;

    /** Lista dei livelli caricati nella fase iniziale */
    queue_t * levels;

    /** Personaggio controllato dall'utente */
    character_t * user;

    /** Lista dei bonus caricati nella fase iniziale */
    list_t * powerups;

    /** Immagine rappresentante le vite del personaggio */
    image_t * life;
    
    /** Tabella degli handler degli eventi */
    hashtable_t * events_table;
    
    /** Coda degli eventi da gestire */
    queue_t * events_queue;

    /** Timer che regola la comparsa dei bonus */
    ttimer_t * powerups_timer;

    /** Timer per gestire le animazioni dei personaggi */
    ttimer_t * characters_timer;

    /** Animazioni di fading-out utilizzate al termine del gioco */
    animation_t * transitions[GAME_ANIMATION_LAST];

    /** Animazioni da animare automaticamente ad ogni loop */
    list_t * animations;

    /** File audio */
    hashtable_t * audio_samples;
    
    /** Muto */
    bool mute;
    
};

/**
 *  Notifica il cambiamento di stato del gioco
 *
 *  @param g Contesto di gioco
 */
#define game_status_changed(g) event_raise(g, EVENT_GAME_STATUS_CHANGED, NULL)

/**
 *  Verifica che il gioco sia stato avviato
 *
 *  @param g Contesto di gioco
 */
#define game_is_started(g) (g && (g->state & GAME_STATE_STARTED))

/**
 *  Verifica che il gioco non sia in pausa o terminato
 *
 *  @param g Contesto di gioco
 */
#define game_is_running(g) (g && (g->state & GAME_STATE_RUNNING))

/**
 *  Verifica che il gioco sia terminato
 *
 *  @param g Contesto di gioco
 */
#define game_is_over(g) (g && (g->state & GAME_STATE_OVER))

/**
 *  Verifica se il gioco è stato vinto
 *
 *  @param g Contesto di gioco
 */
#define game_is_won(g) (g && (g->state & GAME_STATE_WON))

/**
 *  Verifica che il gioco sia in pausa
 *
 *  @param g Contesto di gioco
 */
#define game_is_paused(g) (g && !(g->state & GAME_STATE_RUNNING))

/**
 *  Attiva uno stato
 *
 *  @param game Contesto di gioco
 *  @param state Stato
 */
void game_set_state(game_t * game, int state);

/**
 *  Disattiva uno stato
 *
 *  @param game Contesto di gioco
 *  @param state Stato
 */
void game_unset_state(game_t * game, int state);

/**
 *  Avvia il gioco
 *
 *  @param game Contesto di gioco
 */
void game_set_started(game_t * game);

/**
 *  Riavvia il gioco
 *
 *  @param g Contesto di gioco
 */
#define game_set_running(g)  game_set_state(g, GAME_STATE_RUNNING)

/**
 *  Mette in pausa il gioco
 *
 *  @param g Contesto di gioco
 */
#define game_set_paused(g)  game_unset_state(g, GAME_STATE_RUNNING)

/**
 *  Personaggio controllato dall'utente
 *
 *  @param game Contesto di gioco
 */
character_t * game_get_user(game_t * game);

/**
 *  Livello attualmente in gicoo
 *
 *  @param game Contesto di gioco
 *
 *  @return Livello
 */
level_t * game_get_current_level(game_t * game);

/**
 *  Mette in pausa il gioco se non lo è e viceversa
 *
 *  @param game Contesto di gioco
 */
void game_toggle_pause(game_t * game);

/**
 *  Termina il gioco
 *
 *  @param game Contesto di gioco
 *  @param won Esito del gioco, true in caso di vittoria dell'utente
 */
void game_set_over(game_t * game, bool won);

/**
 *  Inizializzazione del gioco eseguendo il caricamento delle informazioni contenute
 *  nel file di configurazione principale:
 *  - Livelli
 *  - Bonus
 *  - Personaggi
 *
 *  @param config Hashtable contenente la configurazione
 *
 *  @return Contesto di gioco (game_t *)
 *
 *  @retval NULL In caso di errori
 */
game_t * game_initialize(hashtable_t * config);

/**
 *  Deallocazione del contesto di gioco
 *
 *  @param game Contesto di gioco
 */
void game_destroy(game_t * game);

/**
 *  Livello corrente
 *
 *  @return Livello corrente
 */
level_t * game_get_current_level(game_t * game);

#endif
