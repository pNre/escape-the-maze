#include <assert.h>
#include <limits.h>
#include <math.h>

#include "utils.h"

#include "game/animations.h"
#include "main/drawing.h"
#include "main/graphics.h"
#include "main/output.h"

#include "misc/random.h"

#include "game/game.h"
#include "game/map.h"

/**
 *  Ogni tick del timer principale aggiorna lo stato delle animazioni registrate
 */
void animations_handle_animate(game_t * game, event_t event)
{

    if (!TimerEqualToTimer(main_context.timer, event.object))
        return;

    //  aggiornamento dell'animazioni in coda +1 frame/loop
    animations_animate(game);

}

void animations_initialize(game_t * game)
{

    //  inizializza la lista delle animazioni registrate
    game->animations = list_new(animation_functions);

    //  per l'aggiornamento delle animazioni globali
    event_subscribe(game, EVENT_TIMER_TICK, animations_handle_animate);

}

void animations_destroy(game_t * game)
{

    list_delete(game->animations);

}

void animations_animate(game_t * game)
{

    foreach(game->animations, animation_t *, animation) {

        //  solo per le animazioni attive
        if (animation_is_running(animation)) {
            
            if (animation->chain) {
                animation_t * first = list_head(animation->chain);
                
                if (first)
                    animation = queue_head(first->in_chain);

            }
            
            if (animation)
                animation->methods.animate(game, animation);
        }

    }

}

void animation_set_key(animation_t * animation, unsigned int key)
{

    animation->key = key;

}

unsigned int animation_set_random_key(animation_t * animation)
{

    return (animation->key = random_int(0, UINT_MAX - 1));

}

void animation_animate_progress(game_t * game, animation_t * animation) {

    //  se non è una catena
    if (!animation->chain) {
        
        float total_frames = (animation->frames_count * animation->loops_count);
        
        if (animation->frame >= total_frames) {
            animation_end(game, animation);
        }
        
    } else {
        
        //  si controlla lo stato dell'animazione corrente
        animation_t * first = list_head(animation->chain);
        
        assert(first != NULL);
        
        animation_t * current = queue_head(first->in_chain);
        
        assert(current != NULL);
        
        float total_frames = (current->frames_count * current->loops_count);
        
        if (current->frame >= total_frames) {
            
            //  se è terminata si estrae dalla coda
            queue_pop(first->in_chain);
            
            //  se era l'ultima è terminata tutta l'animazione
            if (queue_empty(first->in_chain))
                animation_end(game, first);
            else if (first != current)
                animation_end(game, current);
         
            //  prossima
            animation_t * next = queue_head(first->in_chain);
            
            if (next)
                event_raise(game, EVENT_ANIMATION_BEGAN, next);
            
        }
        
    }
    
}

void animation_animate_fading_in(game_t * game, animation_t * animation)
{

    character_t * user = game_get_user(game);

    rectangle_t rect;
    rect.size = SizeMultiplyBySize(user->map->size, CellSize);
    rect.origin = PointZero;

    float total_frames = (animation->frames_count * animation->loops_count);
    float alpha = animation->frame++ / total_frames;

    color_t color = ColorBackground;
    color.a = alpha * 255;
    color.r *= (1. - alpha);
    color.g *= (1. - alpha);
    color.b *= (1. - alpha);

    rectangle_draw(rect, color);

    animation_animate_progress(game, animation);
    
}

void animation_animate_fading_out(game_t * game, animation_t * animation)
{

    character_t * user = game_get_user(game);

    rectangle_t rect;
    rect.size = SizeMultiplyBySize(user->map->size, CellSize);
    rect.origin = PointZero;

    float total_frames = (animation->frames_count * animation->loops_count);
    float alpha = 1. - (animation->frame++ / total_frames);

    color_t color = ColorBackground;
    color.a = alpha * 255;
    color.r *= (1. - alpha);
    color.g *= (1. - alpha);
    color.b *= (1. - alpha);

    rectangle_draw(rect, color);

    animation_animate_progress(game, animation);
    
}

void animation_reset(animation_t * animation) {
    
    animation->frame = 0;
    animation->frames_left = animation->loops_count * animation->frames_count;

}

void animation_start(game_t * game, animation_t * animation)
{

    //  se è in ferma si riprende dall'inizio
    if (animation->status == ANIMATION_STATUS_STOPPED) {
        animation_reset(animation);
    }

    if (animation->in_chain) {
        queue_push(animation->in_chain, animation);
        foreach(animation->chain, animation_t *, chained_animation) {
            queue_push(animation->in_chain, chained_animation);
            animation_reset(chained_animation);
        }
    }
    
    animation->status = ANIMATION_STATUS_RUNNING;

    event_raise(game, EVENT_ANIMATION_BEGAN, animation);

}

void animation_stop(animation_t * animation)
{
    animation->status = ANIMATION_STATUS_STOPPED;

}

void animation_pause(animation_t * animation)
{
    animation->status = ANIMATION_STATUS_PAUSED;
}

void animation_end(game_t * game, animation_t * animation)
{

    debugf("[Animazione] %u terminata\n", animation->key);

    animation_stop(animation);
    event_raise(game, EVENT_ANIMATION_ENDED, animation);

}

image_t * animation_frame(animation_t * animation)
{

    //  controllo dello stato, se l'animazione è ferma ritorna null
    if (animation->status == ANIMATION_STATUS_RUNNING) {

        //  frame da disegnare
        return animation->frames[animation->frame];

    }

    return animation->frames[1];

}

void animation_animate_frame_change(game_t * game, animation_t * animation)
{

    if (animation->status == ANIMATION_STATUS_RUNNING) {

        //  avanza
        animation->frame = (animation->frame + 1) % animation->frames_count;

        if (!animation->frames_left) {
            //  fine animazione
            animation_end(game, animation);
        }

        if (animation->frames_left > 0)
            animation->frames_left--;

    }

}

void animation_chain(animation_t * head, animation_t * tail) {
    
    //  non si può accodare un'animazione a se stessa
    if (head == tail)
        return;
    
    //  è il primo elemento accodato, vanno create lista e coda
    if (!head->chain) {
        head->chain = list_new(animation_functions);
        head->in_chain = queue_new();
        list_insert(head->chain, head, INSERT_MODE_TAIL, false, false);
    }
    
    //  la nuova animazione si aggiunge in fondo alla lista
    list_insert(head->chain, tail, INSERT_MODE_TAIL, false, true);
 
    //  si mantiene in ogni nodo un riferimento alla lista
    tail->chain = head->chain;
    
}

animation_t * _animation_new(unsigned int loops, int count, image_t * frames[], void (* custom_animate)(game_t *, animation_t *), unsigned int key)
{

    animation_t * animation = memalloc(animation_t);

    //  numero di frames che compongono l'animazione
    animation->frames_count = count;

    //  chiave, di default a 0
    animation->key = key;

    //  frames
    animation->frames = frames;

    //  numero di volte che l'animazione deve essere eseguita
    animation->loops_count = loops;

    //  prossimo frame da mostrare (il primo)
    animation->frame = 0;

    //  numero di loop che rimanongo alla fine dell'animazione
    animation->frames_left = loops * count;

    //  stato iniziale (in animazione, fermo, in pausa)
    animation->status = ANIMATION_STATUS_PAUSED;

    //  inizialmente non c'è nessuna lista
    animation->in_chain = animation->chain = NULL;
    
    //  funzioni di gestione dell'animazione
    animation->methods.frame = animation_frame;
    animation->methods.animate = custom_animate ? custom_animate : animation_animate_frame_change;

    return animation;

}

void animation_delete(animation_t * animation)
{

    //  se è una catena di animazioni si eliminano prima tutte le seguenti
    if (animation->in_chain) {
        list_delete(animation->chain);
        queue_delete(animation->in_chain);
    }
    
    //  i frame vanno deallocati quando si deallocano i personaggi, quindi basta dellocare
    memfree(animation);

}

void animation_enlist(game_t * game, animation_t * animation)
{

    list_insert(game->animations, animation, INSERT_MODE_TAIL, false, true);

}

void animation_delist(game_t * game, animation_t * animation)
{

    list_remove(game->animations, animation);

}

TYPE_FUNCTIONS_DEFINE(animation, animation_delete);

