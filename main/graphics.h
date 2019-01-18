#ifndef main_graphics_h
#define main_graphics_h

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>

#include "std/hashtable.h"

#include "main/output.h"
#include "main/timer.h"

/**
 *  Struttura che impacchetta i riferimenti ad oggetti
 *  necessari alla liberia grafica come lo schermo e la coda degli eventi
 */
typedef struct graphic_context_s {

    /** Display */
    ALLEGRO_DISPLAY * display;

    /** Coda degli eventi della libreria grafica */
    ALLEGRO_EVENT_QUEUE * event_queue;

    /** Font per la scrittura 0: dimensioni normali, 1: x2 */
    ALLEGRO_FONT * font[2];

    /** Timer globale utilizzato per gestire gli eventi */
    ttimer_t * timer;

    /** Origine degli assi */
    point_t origin;

} graphic_context_t;

extern graphic_context_t main_context;

/**
 *  Inizializza la libreria grafica
 *
 *  @return Esito dell'inizializzazione
 *  @retval x < 0 Inizializzazione fallita
 *  @retval 0 Inizializzazione eseguita con successo
 */
int graphics_initialize_library(void);

/**
 *  Inizializza il gioco a partire dal file di configurazione principale
 *
 *  @param config Hashtable contenente le informazioni del file di configurazione principale
 *
 *  @return Esito dell'inizializzazione
 *  @retval x < 0 Inizializzazione fallita
 *  @retval 0 Inizializzazione eseguita con successo
 */
int graphics_initialize(hashtable_t * config);

/** 
 *  Dealloca le strutture allocate in fase di inizializzazione 
 */
void graphics_destroy(void);

/** 
 *  Dimensione dello schermo 
 *
 *  @return Dimensione dello schermo
 */
dimension_t graphics_get_screen_size(void);

/**
 *  Carica un font e lo rende il font di default
 *
 *  @param path Path della bitmap del font
 */
void graphics_load_font(char * path);

/**
 *  Scambia font principale e font secondario
 */
void graphics_fonts_swap(void);

#endif
