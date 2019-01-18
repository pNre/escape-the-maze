#include "main/graphics.h"
#include "main/timer.h"

#include "game/character.h"

ttimer_t * ttimer_new(float freq)
{

    //  creazione timer
    ttimer_t * timer = al_create_timer(freq);

    //  registrazione sorgente eventi
    al_register_event_source(main_context.event_queue, al_get_timer_event_source(timer));

    //  avvio del timer
    al_start_timer(timer);

    return timer;

}

void ttimer_delete(ttimer_t * timer)
{

    al_destroy_timer(timer);

}
