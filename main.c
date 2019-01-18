#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>

#include "config/config.h"

#include "game/game.h"
#include "game/events.h"
#include "game/intro.h"
#include "game/map.h"

#include "main/audio.h"
#include "main/drawing.h"
#include "main/fs.h"
#include "main/graphics.h"

#include "misc/directions.h"
#include "misc/random.h"

int main(int argc, const char * argv[])
{

    //  inizializzazione libreria grafica
    if (graphics_initialize_library() != 0) {
        errorf("Errore in fase di inizializzazione della libreria grafica.\n", NULL);
        return EXIT_FAILURE;
    }

    //  caricamento file principale di configurazione
    char * absolute_config = fs_construct_path(fs_get_resources_path(), "assets", "config.cfg");
    hashtable_t * config = config_open(absolute_config);
    
    memfree(absolute_config);
    
    //  problemi di caricamento del file di config
    if (!config)
        return EXIT_FAILURE;

    //  inizializzazione della libreria grafica
    if (graphics_initialize(config) != 0) {
        
        hashtable_delete(config);
        
        fs_destroy();
        
        return EXIT_FAILURE;
        
    }
    
    //  generatore di numeri random
    well512_initialize();
    
    //  schermata di caricamento
    output_screen_fill(ColorMakeRGB(0, 0, 0));
    output_screen_redraw();

    //  inizializzazione elementi del gioco
    game_t * game = game_initialize(config);

    if (!game) {
        
        graphics_destroy();
        
        hashtable_delete(config);
        
        fs_destroy();
        
        return EXIT_FAILURE;
        
    }
    
    //  menu principale
    bool done = !intro_display(game);

    while (!done) {

        event_t event;

        //  gestisce gli eventi
        int events = events_handle(game, EVENT_EXIT | EVENT_GAME_RELOAD, &event);

        //  c'è stato un EVENT_EXIT?
        if (events & EVENT_EXIT) {
            done = true;
        } else if (events & EVENT_GAME_RELOAD) {
            
            //  salvataggio dello stato dell'audio
            bool mute = game->mute;
            
            //  libera le risorse
            game_destroy(game);

            //  inizializzazione elementi del gioco
            game = game_initialize(config);
            game->mute = mute;
            
            //  ri-avvio il gioco
            game_set_started(game);

        }

        //  fading out delle tracce audio
        audio_fade(game);

    }

    game_destroy(game);

    graphics_destroy();

    fs_destroy();
    
    //  deallocazione file di config
    hashtable_delete(config);
    
    return EXIT_SUCCESS;
}



