#ifndef game_animations_h
#define game_animations_h

#include <limits.h>

#include "misc/geometry.h"

#include "main/timer.h"
#include "main/image.h"

#include "game/structs.h"

#include "std/list.h"
#include "std/queue.h"

/** Stato dell'animazione */
typedef enum animation_status_t {

    ANIMATION_STATUS_RUNNING,
    ANIMATION_STATUS_PAUSED,
    ANIMATION_STATUS_STOPPED,

} animation_status_t;

/**
 *  Animazione
 */
struct animation_s {

    /** Frames che costituiscono l'animazione */
    image_t ** frames;

    /** Numero di frames che costituiscono l'animazione */
    int frames_count;

    /** Numero di volte che l'animazione deve essere eseguita */
    unsigned int loops_count;

    /** Frame corrente dell'animazione */
    unsigned int frame;

    /** Frame ancora da mostrare */
    unsigned long long frames_left;

    /** Stato dell'animazione */
    animation_status_t status;

    /** Chiave identificativa */
    unsigned int key;
    
    /** Lista delle animazioni da eseguire */
    list_t * chain;
    
    /** Coda delle animazioni ancora da eseguire */
    queue_t * in_chain;
    
    /** Metodi per la gestione dell'animazione */
    struct {

        /** Puntatore ad una funzione che si occupa di aggiornare lo stato 
            dell'animazione (es. aggiornare l'indice dal frame corrente) */
        void (* animate)(game_t *, struct animation_s *);

        /** Puntatore ad una funzione che si occupa di ritornare il frame corrente */
        image_t * (* frame)(struct animation_s *);

    } methods;

};

/** Massimo numero di loop (virtualmente infiniti) */
#define ANIMATE_FOREVER    UINT_MAX

/**
 *  Inizializza le strutture di supporto alle animazioni
 *
 *  @param game Contesto di gioco
 */
void animations_initialize(game_t * game);

/**
 *  Libera la memoria allocata in fase di inizializzazione
 *
 *  @param game Contesto di gioco
 */
void animations_destroy(game_t * game);

/**
 *  Avanza lo stato delle animazioni registrate
 *
 *  @param game Contesto di gioco
 */
void animations_animate(game_t *);

/**
 *  Crea una nuova animazione dati un certo numero di parametri
 *
 *  @param loops Numero di volte che l'animazione deve essere eseguita
 *  @param count Numero di frames che compongono l'animazione
 *  @param frames Frames che compongono l'animazione
 *  @param custom_animate Puntatore ad una funzione che si occupa di gestire l'animazione
 *
 *  @return Animazione
 */
#define animation_new(...)                   OVERLOAD(animation_new_, __VA_ARGS__)

#define animation_new_1(loops)                                       _animation_new(loops, 0, NULL, NULL, 0)
#define animation_new_3(loops, count, frames)                        _animation_new(loops, count, frames, NULL, 0)
#define animation_new_4(loops, count, frames, custom_animate)        _animation_new(loops, count, frames, custom_animate, 0)
#define animation_new_5(loops, count, frames, custom_animate, key)   _animation_new(loops, count, frames, custom_animate, key)

animation_t * _animation_new(unsigned int loops, int count, image_t * frames[], void (* custom_animate)(game_t *, animation_t *), unsigned int key);

/**
 *  Dealloca un'animazione
 *
 *  @param animation Animazione
 */
void animation_delete(animation_t * animation);

/**
 *  Imposta una chiave identificativa per un'animazione
 *
 *  @param animation Animazione
 *  @parma key Chiave
 */
void animation_set_key(animation_t * animation, unsigned int key);

/**
 *  Imposta una chiave identificativa casuale per un'animazione
 *
 *  @param animation Animazione
 *
 *  @return Chiave
 */
unsigned int animation_set_random_key(animation_t * animation);

/**
 *  Aggiunge un'animazione, che sarà animata automaticamente, alla lista delle animazioni globali
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione
 */
void animation_enlist(game_t * game, animation_t * animation);

/**
 *  Rimuove un'animazione dalla lista delle animazioni globali
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione
 */
void animation_delist(game_t * game, animation_t * animation);

/**
 *  Avvia un'animazione
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione
 */
void animation_start(game_t * game, animation_t * animation);

/**
 *  Cancella un'animazione
 *
 *  @param animation Animazione
 */
void animation_stop(animation_t * animation);

/**
 *  Mette in pausa un'animazione
 *
 *  @param animation Animazione
 */
void animation_pause(animation_t * animation);

/**
 *  Termina definitivamente un'animazione
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione
 */
void animation_end(game_t * game, animation_t * animation);

/**
 *  Concatena due animazioni
 *
 *  @param head Animazione da eseguire per prima
 *  @param tail Animazione da eseguire per ultima
 */
void animation_chain(animation_t * head, animation_t * tail);

/**
 *  Regola lo stato di avanzamento di un'animazione ed eventualmente la ferma o passa alla successiva
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione di riferimento
 */
void animation_animate_progress(game_t * game, animation_t * animation);

/**
 *  Anima il fading-in dello schermo.
 *  La durata dell'animazione è determinata in base al numero di frames
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione di riferimento
 */
void animation_animate_fading_in(game_t * game, animation_t * animation);

/**
 *  Anima il fading-out dello schermo
 *
 *  @param game Contesto di gioco
 *  @param animation Animazione di riferimento
 */
void animation_animate_fading_out(game_t * game, animation_t * animation);

/**
 *  Controlla che un'animazione sia in esecuzione
 *
 *  @param a Animazione
 */
#define animation_is_running(a)    (a && a->status == ANIMATION_STATUS_RUNNING)

/**
 *  Controlla che un'animazione sia ferma
 *
 *  @param a Animazione
 */
#define animation_is_stopped(a)    (a && a->status == ANIMATION_STATUS_STOPPED)

#endif
