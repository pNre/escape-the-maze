#include "game/events.h"
#include "game/game.h"
#include "game/intro.h"

#include "main/audio.h"
#include "main/drawing.h"
#include "main/graphics.h"
#include "main/output.h"

#include "misc/random.h"

#include "io/keyboard.h"

void intro_redraw(game_t * game, image_t * logo, char ** menu_items, int menu_items_count, int selected_index)
{

    dimension_t screen = graphics_get_screen_size();
    dimension_t scale = SizeMake(2., 2.);
    
    output_set_scale(scale);

    //  margine tra le opzioni
    float margin = 5.;

    //  calcolo dell'origine del messaggio
    point_t point = PointZero;

    point.x = (screen.width / scale.width - logo->size.width) / 2.;
    point.y = (screen.height / scale.height - logo->size.height) / 2.;
    output_image(logo, point);

    //  origine
    point.y += logo->size.height + 20.;

    int i;
    for (i = 0; i < menu_items_count; i++) {

        dimension_t size = output_get_string_size(menu_items[i]);
        point.x = (screen.width / scale.width - size.width) / 2.;

        color_t color = (i == selected_index) ? ColorText2 : ColorText1;

        output_stringf(color, point, menu_items[i]);
        point.y += size.height + margin;

    }

    output_screen_redraw();

}

/**
 *  Gestione dell'evento EVENT_ANIMATION_ENDED
 */
void intro_handle_animation_ended_event(game_t * game, event_t event)
{

    animation_t * animation = event.object;

    if (animation->key != 0xFEE1DEAD)
        return;

    //  rimozione animazione dalla lista
    animation_delist(game, animation);

}

bool intro_display(game_t * game)
{

    enum {
      
        MENU_ITEM_START,
        MENU_ITEM_TOGGLE_AUDIO,
        MENU_ITEM_EXIT
        
    };
    
    //  voci del menù
    char * menu_items[] = {
        "Avvia il gioco",
        "Disattiva Audio",
        "Esci"
    };

    //  inizia la riproduzione della traccia audio della scherma iniziale
    audio_sample_play_by_id(game, AUDIO_SAMPLE_INTRO, true);

    //  logo
    image_t * logo = image_load_new("assets/logo.bmp", RectNull, false);

    //  evento fine animazione
    event_subscribe(game, EVENT_ANIMATION_ENDED, intro_handle_animation_ended_event);

    //  animazione di fading in nella mappa
    animation_t * fading_in = animation_new(1, 30, NULL, animation_animate_fading_in);
    animation_set_key(fading_in, 0xFEE1DEAD);
    animation_enlist(game, fading_in);

    //  opzione selezionata
    int selected_index = 0;

    //  numero delle voci del menù
    const int menu_items_count = sizeof(menu_items) / sizeof(char *);

    bool done = false;

    do {

        event_t event;

        int events = events_handle(game, EVENT_KEY_DOWN | EVENT_EXIT, &event);
        
        if (events & EVENT_KEY_DOWN) {

            if (event.data == KYB_KEY_ENTER) {
                //  avvia il gioco/esci
                if (selected_index == MENU_ITEM_START || selected_index == MENU_ITEM_EXIT) {
                    done = true;
                }
                //  audio on/off
                else if (selected_index == MENU_ITEM_TOGGLE_AUDIO)
                {
                    audio_toggle_mute(game);
                    menu_items[selected_index] = game->mute ? "Attiva Audio" : "Disattiva Audio";
                }
            } else if (event.data == KYB_KEY_DOWN) {
                selected_index = (selected_index + 1) % menu_items_count;
            } else if (event.data == KYB_KEY_UP) {
                selected_index = (selected_index - 1) < 0 ? (menu_items_count - 1) : (selected_index - 1);
            }

        } else if (events & EVENT_EXIT) {
            
            done = true;
            selected_index = MENU_ITEM_EXIT;
            
        }

        intro_redraw(game, logo, menu_items, menu_items_count, selected_index);

    } while (!done);
    
    image_delete(logo);
    
    if (selected_index == MENU_ITEM_EXIT)
        return false;
        
    //  avvio del gioco
    game_set_started(game);
    
    //  avvio fading in della mappa
    animation_start(game, fading_in);

    //  ferma la riproduzione della traccia audio
    audio_sample_set_fading_by_id(game, AUDIO_SAMPLE_INTRO);

    return true;

}
