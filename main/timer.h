#ifndef main_timer_h
#define main_timer_h

#include <allegro5/allegro5.h>
#include <stdint.h>

#include "game/structs.h"

//  Frequenza del timer principale
#define TIMER_FREQUENCY (1. / 60)

//  Frequenza del timer che anima i personaggi
#define CHARACTERS_TIMER_FREQUENCY (1. / 5)

/**
 *  Timer, ridefinito dalla libreria grafica
 */
typedef ALLEGRO_TIMER ttimer_t;

/**
 *  Creazione di un nuovo timer
 *
 *  @param freq Frequenza del timer, in secondi
 *
 *  @return Timer
 */
ttimer_t * ttimer_new(float freq);

/**
 *  Deallocazione di un timer
 *
 *  @param timer Timer da eliminare
 */
void ttimer_delete(ttimer_t * timer);

/**
 *  Confronta due timer
 */
#define TimerEqualToTimer(t1, t2)       (t1 == t2)

#endif
