#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "main/drawing.h"
#include "main/graphics.h"

#include "game/game.h"
#include "game/level.h"
#include "game/map.h"
#include "game/powerup.h"

#include "config/config.h"

#include "misc/random.h"

#include "utils.h"

/**
 *  Esegue l'avanzamento al livello successivo
 */
void game_perform_level_transition(game_t * game) {
    
    //  si estrae il livello attuale dalla coda
    level_t * level = queue_pop(game->levels);
    level_uninstall(game, level);
    level_delete(level);
    
    //  nuovo livello corrente
    level = game_get_current_level(game);
    level_setup(game, level);
    
    character_decide_direction_user(game_get_user(game));

}

/**
 *  Handler per l'evento EVENT_GAME_LOST
 */
void game_handle_lost(game_t * game, event_t evt) {
    
    game_set_over(game, false);
    animation_start(game, game->transitions[GAME_ANIMATION_OVER]);
    
}

/**
 *  Handler per l'evento EVENT_GAME_WON
 */
void game_handle_won(game_t * game, event_t evt) {
    
    game_set_over(game, true);  //  vittoria
    animation_start(game, game->transitions[GAME_ANIMATION_WON]);

}

/**
 *  Handler per l'evento EVENT_LEVEL_NEXT, eseguito al cambio di livello
 */
void game_handle_next_level(game_t * game, event_t evt)
{

    character_t * character = evt.object;

    if (!character)
        return;

    //  l'utente è arrivato per primo a fine livello
    if (character->is_user) {

        //  se c'è un livello dopo quello attuale, si avanza
        if (list_length(game->levels) > 1) {

            game_set_paused(game);

            animation_start(game, game->transitions[GAME_ANIMATION_LEVEL_NEXT]);

        } else {    //  in caso contrario si è arrivati alla fine del gioco

            event_raise(game, EVENT_GAME_WON, NULL);
            
        }

    } else {

        //  un avversario è arrivato per primo
        //  si avvia l'animazione di fine gioco
        event_raise(game, EVENT_GAME_LOST, NULL);

    }
}

/**
 *  Handler per l'evento EVENT_GAME_OVER
 */
void game_handle_status_changed(game_t * game, event_t evt)
{

    if (game_is_paused(game) && !game_is_over(game))
    {
        
        //  disabilitazione animazioni
        character_animations_stop(game->user);

        //  livello corrente
        level_t * level = game_get_current_level(game);
        
        foreach(level->enemies, character_t *, character)
        {
            character_animations_stop(character);
        }

    }

}

/**
 *  Didascalia con il nome del livello mostrata quando l'utente 
 *  raggiunge la cella di fine mappa dell'ultima mappa di un livello.
 *  In realtà c'è poco da animare, viene semplicemente riscritto il nome del livello per 90 loops.
 */
void game_animate_level_transition(game_t * game, animation_t * animation) {
    
    animation->frame++;
    
    output_screen_fill(ColorBackground);
    
    //  livello corrente
    level_t * level = game_get_current_level(game);

    output_fullscreen_message(level->name, "", ColorText2);
    
    animation_animate_progress(game, animation);
    
}

/**
 *  Handler per l'evento EVENT_ANIMATION_BEGAN, eseguito all'inizio di un'animazione.
 */
void game_handle_animation_began_event(game_t * game, event_t evt)
{
    animation_t * animation = (animation_t *)evt.object;
    
    if (animation->key == LEVEL_NEXT_TRANSITION_ANIMATION) {

        game_perform_level_transition(game);
        
    }
}
    
/**
 *  Handler per l'evento EVENT_ANIMATION_ENDED, eseguito al termine di un'animazione.
 */
void game_handle_animation_ended_event(game_t * game, event_t evt)
{

    animation_t * animation = (animation_t *)evt.object;
    
    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    //  animazioni di vittoria/sconfitta
    if (animation == game->transitions[GAME_ANIMATION_OVER] ||
        animation == game->transitions[GAME_ANIMATION_WON]) {

        //  se c'è una traccia audio per il livello
        audio_sample_t * level_track = hashtable_search(game->audio_samples, level->name);

        //  si ferma la riproduzione
        if (level_track)
            audio_sample_stop(level_track);

        //  e si avvia il suono di vittoria/sconfitta
        if (evt.object == game->transitions[GAME_ANIMATION_OVER])
            audio_sample_play_by_id(game, AUDIO_SAMPLE_GAMEOVER, false);
        else
            audio_sample_play_by_id(game, AUDIO_SAMPLE_GAMEWON, false);

    }
    //  didascalia con il nome del livello
    //  quando scompare comincia il fading-in del livello successivo
    else if (animation->key == LEVEL_NEXT_TRANSITION_ANIMATION) {

        game_set_running(game);
        
    }
    
}

/**
 *  Handler per l'evento EVENT_KEY_DOWN
 */
void game_handle_key_down(game_t * game, event_t evt)
{

    switch (evt.data) {
        //  pausa
        case KYB_KEY_SPACE:
            game_toggle_pause(game);
            break;
            
        //  uscita
        case KYB_KEY_ESCAPE:
            event_raise(game, EVENT_EXIT, NULL);
            break;
            
        //  muto
        case KYB_KEY_1:
            audio_toggle_mute(game);
            break;
        
        //  riavvio il gioco in caso di game over
        case KYB_KEY_ENTER:
            
            if (game_is_over(game))
                event_raise(game, EVENT_GAME_RELOAD, NULL);
            
            break;
            
        default:
            break;
    }
    
}

/**
 *  Schermata di game over
 *
 *  @param game Contesto di gioco
 */
void game_over(game_t * game)
{
    output_screen_fill(ColorBlack);
    output_fullscreen_message("Game Over", "[Invio] per ricominciare", ColorRandom(127));
    output_screen_redraw();

}

/**
 *  Schermata di gioco completato
 *  @param game Contesto di gioco
 */
void game_won(game_t * game)
{
    
    output_screen_fill(ColorBackground);
    output_fullscreen_message("Vittoria", "[Invio] per ricominciare", ColorRandom(127));
    output_screen_redraw();

}

/**
 *  Handler per il timer che si occupa di aggiornare lo schermo ogni 60-esimo di secondo
 */
void game_handle_redraw(game_t * game, event_t e)
{

    if (!TimerEqualToTimer(main_context.timer, e.object))
        return;

    if (!game_is_started(game))
        return;

    if (game_is_over(game)) {

        if (!game_is_won(game) && animation_is_stopped(game->transitions[GAME_ANIMATION_OVER])) {

            game_over(game);
            
            return;

        } else if (game_is_won(game) && animation_is_stopped(game->transitions[GAME_ANIMATION_WON])) {

            game_won(game);
            
            return;
        }

    }

    //  sfondo
    output_screen_fill(ColorBackground);

    character_t * user = game_get_user(game);

    //  ridisegna la mappa
    map_update(game, user->map);

    //  se il gioco è in pausa il personaggio non può essere controllato
    if (game_is_running(game))
        user->methods.control(game, user);

    //  ridisegna il personaggio
    character_redraw(game, user);

    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    foreach(level->enemies, character_t *, character) {

        if (game_is_running(game))
            character->methods.control(game, character);

        character_redraw(game, character);

    }

    //  area visibile
    dimension_t visible_area_size = dimension_value(hashtable_search(user->config, "visible_area_size"));
    dimension_t map_size = SizeMultiplyBySize(user->map->size, CellSize);
    
    //  se l'area visibile non è grande infinito, bisogna oscurare alcune parti della mappa
    if (!SizeIsInfinity(visible_area_size)) {
        
        visible_area_size = SizeMultiplyBySize(visible_area_size, CellSize);
        
        //  centro del riquadro
        point_t center = PointOffset(user->position, CellSize.width / 2., CellSize.height / 2.);

        //  rettangolo di visibilità
        rectangle_t viewable_rect = rectangle_centered_make(center, visible_area_size, map_size);
        
        //  aree da oscurare
        rectangle_t fog[] = {
            RectMake(0, 0, map_size.width, RectGetMinY(viewable_rect)),
            RectMake(0, RectGetMaxY(viewable_rect), map_size.width, map_size.height - RectGetMaxY(viewable_rect)),
            RectMake(0, RectGetMinY(viewable_rect), RectGetMinX(viewable_rect), viewable_rect.size.height),
            RectMake(RectGetMaxX(viewable_rect), RectGetMinY(viewable_rect), map_size.width - RectGetMaxX(viewable_rect), viewable_rect.size.height)
        };
        
        unsigned int i;
        for (i = 0; i < (sizeof(fog) / sizeof(rectangle_t)); i++) {
            rectangle_draw(fog[i], ColorMakeRGBA(0, 0, 0, 15));
        }
        
    }
    
    //  per indicare che il gioco è in pausa si sovrappone un livello di grigio
    if (game_is_paused(game))
        rectangle_draw(RectMake(0, 0, map_size.width, map_size.height), ColorMakeRGBA(30, 30, 30, 140));
    
    //  lo schermo viene aggiornato al prossimo loop in modo tale
    //  da assicurarsi che le animazioni siano state aggiornate
    event_raise(game, EVENT_SCREEN_REDRAW, NULL);
    
}

level_t * game_get_current_level(game_t * game)
{
    
    return queue_head(game->levels);
    
}

character_t * game_get_user(game_t * game)
{

    return game->user;

}

game_t * game_initialize(hashtable_t * config)
{
    
    //  creazione struttura game_t
    game_t * game = memalloc(game_t, 1, true);
    
    //  gestore audio
    game->audio_samples = audio_initialize(config);
    game->mute = false;
    
    //  caricamento delle configurazioni
    list_t * levels = levels_load(game, config);

    //  nessun livello caricato
    if (!levels || !list_length(levels)) {
        
        list_delete(levels);
        
        game_destroy(game);
        
        return NULL;
        
    }

    //  impostazione livelli
    game->levels = levels;

    //  caricamento personaggi dell'utente
    hashtable_t * user_character_config = hashtable_search(config, "user");

    if (!user_character_config) {
        game_destroy(game);
        return NULL;
    }

    //  personaggio dell'utente (primo della lista dei personaggi caricati)
    game->user = character_load(user_character_config, 1);

    if (!game->user) {
        game_destroy(game);
        return NULL;
    }

    //  caricamento dei bonus
    char * powerups_file = hashtable_search(config, "powerups");

    if (powerups_file) {

        hashtable_t * powerups_config = config_open(powerups_file);

        if (powerups_config) {

            game->powerups = powerups_load(game, powerups_config);
            hashtable_delete(powerups_config);

        } else {

            game->powerups = NULL;

        }

    }
    
    //  bitmap rappresentante 1 vita
    char * life = hashtable_search(config, "life");
    
    if (life)
        game->life = image_load_new(life, RectNull, true);
    
    //  gestore degli eventi
    events_initialize(game);

    //  installazione handlers di default
    event_subscribe(game, EVENT_SCREEN_REDRAW, output_handle_screen_redraw);
    
    //  60fps
    event_subscribe(game, EVENT_TIMER_TICK, game_handle_redraw);

    //  evento nuovo livello
    event_subscribe(game, EVENT_LEVEL_NEXT, game_handle_next_level);

    //  un avverssario è arrivato alla fine del livello per primo
    event_subscribe(game, EVENT_GAME_STATUS_CHANGED, game_handle_status_changed);

    //  animazione terminata
    event_subscribe(game, EVENT_ANIMATION_BEGAN, game_handle_animation_began_event);
    event_subscribe(game, EVENT_ANIMATION_ENDED, game_handle_animation_ended_event);

    //  eventi da tastiera
    event_subscribe(game, EVENT_KEY_DOWN, game_handle_key_down);
    
    //  vittoria/sconfitta
    event_subscribe(game, EVENT_GAME_WON, game_handle_won);
    event_subscribe(game, EVENT_GAME_LOST, game_handle_lost);

    //  inzializzazione di alcuni timer
    game->powerups_timer = ttimer_new(1);
    game->characters_timer = ttimer_new(CHARACTERS_TIMER_FREQUENCY);
    
    //  animazioni
    animations_initialize(game);

    //  inizializzazioni animazioni
    game->transitions[GAME_ANIMATION_OVER] = animation_new(1, 30, NULL, animation_animate_fading_out);
    game->transitions[GAME_ANIMATION_WON] = animation_new(1, 30, NULL, animation_animate_fading_out);
    
    game->transitions[GAME_ANIMATION_LEVEL_NEXT] = animation_new(1, 30, NULL, animation_animate_fading_out, LEVEL_NEXT_FADING_OUT_ANIMATION);
    animation_chain(game->transitions[GAME_ANIMATION_LEVEL_NEXT], animation_new(1, 90, NULL, game_animate_level_transition, LEVEL_NEXT_TRANSITION_ANIMATION));
    animation_chain(game->transitions[GAME_ANIMATION_LEVEL_NEXT], animation_new(1, 30, NULL, animation_animate_fading_in));
    
    animation_enlist(game, game->transitions[GAME_ANIMATION_OVER]);
    animation_enlist(game, game->transitions[GAME_ANIMATION_WON]);
    animation_enlist(game, game->transitions[GAME_ANIMATION_LEVEL_NEXT]);

    return game;

}

void game_destroy(game_t * game)
{

    if (!game)
        return;

    //  1. deallocazione livelli
    if (game->levels) {
        
        while (!queue_empty(game->levels)) {
            
            level_t * level = queue_pop(game->levels);

            level_delete(level);
        }
        
        queue_delete(game->levels);
        
    }

    //  2. deallocazione personaggio dell'utente
    character_delete(game->user);

    //  3. deallocazione bonus
    list_delete(game->powerups);

    //  4. distruzione timers
    ttimer_delete(game->powerups_timer);
    ttimer_delete(game->characters_timer);

    //  5. animazioni
    animations_destroy(game);

    //  6. audio
    audio_destroy(game->audio_samples);

    //  7. gestore degli eventi
    events_destroy(game);
    
    //  8. vita
    image_delete(game->life);
    
    //  9. deallocazione game_t
    memfree(game);

}

void game_set_state(game_t * game, int state)
{

    game->state |= state;

    //  generazione dell'evento EVENT_GAME_STATUS_CHANGED
    game_status_changed(game);

}

void game_unset_state(game_t * game, int state)
{

    game->state &= ~state;

    //  generazione dell'evento EVENT_GAME_STATUS_CHANGED
    game_status_changed(game);

}

void game_toggle_pause(game_t * game)
{

    game->state ^= GAME_STATE_RUNNING;

    game_status_changed(game);

}

void game_set_over(game_t * game, bool won)
{

    game->state |= GAME_STATE_OVER;

    if (won)
        game->state |= GAME_STATE_WON;
    else
        game->state &= ~GAME_STATE_WON;
    
    game_unset_state(game, GAME_STATE_RUNNING);

    game_status_changed(game);

}

void game_set_started(game_t * game) {
    
    //  primo livello
    level_setup(game, list_head(game->levels));
    
    //  flags di avvio
    game_set_state(game, GAME_STATE_STARTED | GAME_STATE_RUNNING);
    game_unset_state(game, GAME_STATE_OVER);

}
