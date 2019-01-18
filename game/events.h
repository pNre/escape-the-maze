#ifndef game_events_h
#define game_events_h

#include <stdint.h>

#include "game/structs.h"

#include "io/keyboard.h"

#include "main/timer.h"

#include "std/hashtable.h"

/**
 *  Definisce i possibili tipi di evento
 */
enum {

    /** Evento sconosciuto */
    EVENT_UNKNOWN = 1 << 0,

    /** "Tick" del timer */
    EVENT_TIMER_TICK = 1 << 1,

    /** Tasto premuto */
    EVENT_KEY_DOWN = 1 << 2,

    /** Chiusura della finestra principale */
    EVENT_EXIT = 1 << 3,

    /** Utilizza bonus */
    EVENT_POWERUP_USE = 1 << 4,

    /** Prossima mappa */
    EVENT_MAP_NEXT = 1 << 5,

    /** Prossimo livello */
    EVENT_LEVEL_NEXT = 1 << 6,

    /** Animazione avviata */
    EVENT_ANIMATION_BEGAN = 1 << 7,
    
    /** Animazione terminata */
    EVENT_ANIMATION_ENDED = 1 << 8,

    /** Il gioco ha cambiato stato pausa/finito/... */
    EVENT_GAME_STATUS_CHANGED = 1 << 9,
    
    /** Ricarica il gioco */
    EVENT_GAME_RELOAD = 1 << 10,
    
    /** Sconfitta */
    EVENT_GAME_LOST = 1 << 11,
    
    /** Vittoria */
    EVENT_GAME_WON = 1 << 12,
    
    /** Aggiornamento schermo */
    EVENT_SCREEN_REDRAW = 1 << 13
    
};

/**
 *  Contiene le informazioni su un evento
 */
struct event_s {

    /** Tipo di evento */
    uint32_t type;

    /** Oggetto correllato all'evento */
    void * object;

    /** Informazioni aggiuntive */
    int data;

};

/** Tipo degli handler degli eventi */
typedef void (* event_handler_t)(game_t * game, event_t event);

/**
 *  Inizializza il sistema interno di gestione degli eventi
 */
void events_initialize(game_t * game);

/**
 *  Dealloca tutte le strutture utilizzate dal gestore degli eventi
 */
void events_destroy(game_t * game);

/**
 *  Registra una funzione di callback per gestire le notifiche per un certo tipo di evento
 *
 *  @param type Tipo di evento da gestire
 *  @param handler Funzione di callback
 */
void event_subscribe(game_t * game, uint32_t type, event_handler_t handler);

/**
 *  De-registra una funzione di callback
 *
 *  @param type Tipo di evento da gestire
 *  @param handler Funzione di callback
 */
void event_unsubscribe(game_t * game, uint32_t type, event_handler_t handler);

/**
 *  Genera un evento
 *
 *  @param type Tipo di evento da generare
 *  @param object Oggetto correllato all'evento
 */
void event_raise(game_t * game, uint32_t type, void * object);

/**
 *  Gestisce la coda degli eventi, chiama gli handler registrati, e verifica un particolare evento _watch_
 *
 *  @param game Contesto di gioco
 *  @param watch Evento da osservare
 *  @param watched Evento watch se si è verificato
 *
 *  @return Maschera degli eventi osservati
 */
int events_handle(game_t * game, uint32_t watch, event_t * watched);

#endif
