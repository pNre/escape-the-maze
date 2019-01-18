#include <stdint.h>

#include "std/queue.h"

#include "game/character.h"
#include "game/events.h"
#include "game/game.h"
#include "game/powerup.h"

#include "main/graphics.h"

event_t * event_new(uint32_t type)
{

    event_t * event = memalloc(event_t);

    event->type = type;

    return event;

}

void event_dealloc(event_t * event)
{

    memfree(event);

}

int events_handle(game_t * game, uint32_t watch, event_t * watched)
{

    //  maschera degli eventi osservati
    int events_mask = 0;

    //  aspetta un evento della libreria grafica
    ALLEGRO_EVENT allegro_event;
    al_wait_for_event(main_context.event_queue, &allegro_event);

    //  codice evento interno
    uint32_t code = EVENT_UNKNOWN;
    int data = -1;

    //  se l'evento è da tastiera, tasto premuto
    void * source = NULL;

    //  conversione del codice evento
    switch (allegro_event.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            code = EVENT_EXIT;
            break;

        case ALLEGRO_EVENT_TIMER:
            code = EVENT_TIMER_TICK;
            source = allegro_event.timer.source;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            code = EVENT_KEY_DOWN;
            data = allegro_event.keyboard.keycode;
            break;
    }

    //  se è un evento sconosciuto non può essere gestito
    if (code != EVENT_UNKNOWN) {

        //  creazione event_t
        event_t * event = event_new(code);

        //  sorgente evento
        event->object = source;

        //  info
        event->data = data;

        //  inserimento nella coda
        queue_push(game->events_queue, event);

        //  aggiunta agli eventi verificati
        events_mask |= code;

        if ((code & watch) && watched) {

            *watched = *event;

        }

    }

    //  svuotamento della coda
    event_t * event;

    while ((event = (event_t *)queue_pop(game->events_queue)) != NULL) {

        //  cerca gli handler per il tipo di evento
        list_t * handlers = hashtable_chain(game->events_table, &event->type);

        //  aggiunta agli eventi verificati
        events_mask |= event->type;
        
        //  è uno degli eventi osservati?
        if ((event->type & watch) && watched) {
            
            *watched = *event;
            
        }
        
        //  non ce ne sono, prossimo evento
        if (!handlers) {
            
            event_dealloc(event);
            
            continue;
            
        }

        //  chiamata handlers
        foreach(handlers, hashtable_node_t *, node) {

            event_handler_t handler = (event_handler_t)node->value;

            handler(game, *event);

        }

        //  deallocazione evento
        event_dealloc(event);

    }

    return events_mask;

}

/** Funzione di hashing utilizzata per l'inserimento dei dati nella tabella degli handler */
hashtable_hash_t event_hash(void * key, size_t size)
{

    //  La chiave è il tipo di evento, una potenza di 2,
    //  calcolando l'esponente si fa corrispondere
    //  ad ogni cella della tabella handler per un unico tipo di evento
    //
    //  table[0] = /
    //  table[1] = Handlers EVENT_TIMER_TICK
    //  table[2] = Handlers EVENT_KEY_DOWN
    //  ...
    return (int)log2(*((uint32_t *)key));

}

void event_subscribe(game_t * game, uint32_t type, event_handler_t handler)
{

    //  inserimento dell'handler nella tabella hash
    hashtable_insert(game->events_table, &type, (void *)handler, no_functions, 0);

}

void event_unsubscribe(game_t * game, uint32_t type, event_handler_t handler)
{

    //  cerca gli handler per il tipo di evento
    list_t * handlers = hashtable_chain(game->events_table, &type);

    //  non ce ne sono, prossimo evento
    if (!handlers)
        return;

    //  ricerca handler giusto
    hashtable_node_t * node_to_delete = NULL;

    foreach(handlers, hashtable_node_t *, node) {

        if ((event_handler_t)node->value == handler) {
            node_to_delete = node;
            break;
        }

    }

    //  trovato
    if (node_to_delete)
        list_remove(handlers, node_to_delete);

}

void event_raise(game_t * game, uint32_t type, void * object)
{

    //  nuovo evento
    event_t * event = event_new(type);
    event->object = object;

    //  accodamento
    queue_push(game->events_queue, event);

}

void events_initialize(game_t * game)
{

    //  creazione di una hashtable avente come dimensione il numero di eventi
    game->events_table = hashtable_new(sizeof(game->state) * 8, long_functions);

    //  funzione di hashing che ad ogni evento fa corrispondere il tipo
    //  in questo modo in ogni locazione della tabella ci saranno eventi di un solo tipo
    game->events_table->hash = event_hash;

    //  coda degli eventi
    //  quando un evento viene generato gli handler non sono eseguiti automaticamente
    //  l'evento viene quindi accodato e gestito in un secondo momento
    game->events_queue = queue_new();

    //  registrazione handlers
    characters_register_event_handlers(game);
    powerups_register_event_handlers(game);
    levels_register_event_handlers(game);
}

void events_destroy(game_t * game)
{

    //  1. tabella degli eventi
    hashtable_delete(game->events_table);

    //  2. svuotamento coda
    while (!queue_empty(game->events_queue)) {
        event_t * event = (event_t *)queue_pop(game->events_queue);
        event_dealloc(event);
    }
    
    //  3. coda
    queue_delete(game->events_queue);

}
