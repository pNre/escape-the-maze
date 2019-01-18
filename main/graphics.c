#include <stdlib.h>

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>

#include "misc/geometry.h"

#include "main/timer.h"
#include "main/graphics.h"

graphic_context_t main_context;

int graphics_initialize_library(void)
{

    //  inizializzazione libreria
    if (!al_init())
        return -1;

    //  tastiera
    if (!al_install_keyboard())
        return -2;

    //  addon per il caricamento delle immagini
    if (!al_init_image_addon())
        return -3;

    //  addon per il testo
    al_init_font_addon();
    
    //  primitive (funzioni per disegnare rettangoli, ...)
    if (!al_init_primitives_addon())
        return -4;

    //  modalità finestra
    al_set_new_display_flags(ALLEGRO_WINDOWED);

    return 0;
}

int graphics_initialize(hashtable_t * config)
{

    //  dimensione dell'area di gioco
    dimension_t   screen_area_default = SizeMake(800, 600);
    dimension_t * screen_area = hashtable_search(config, "screen");

    //  se nel file di configurazione non ci sono informazioni sulle dimensioni,
    //  si utilizzano quelle di default
    if (!screen_area)
        screen_area = &screen_area_default;

    //  creazione finestra
    main_context.display = al_create_display(screen_area->width, screen_area->height);
    
    if (!main_context.display)
        return -1;
    
    //  titolo
    al_set_window_title(main_context.display, "Gara tra i labirinti");
    
    //  origine degli assi
    main_context.origin = PointZero;
    
    //  coda degli eventi
    main_context.event_queue = al_create_event_queue();
    
    if (!main_context.event_queue)
        return -2;
    
    //  timer
    main_context.timer = ttimer_new(TIMER_FREQUENCY);
    
    //  registrazione delle sorgenti degli eventi
    al_register_event_source(main_context.event_queue, al_get_keyboard_event_source());
    al_register_event_source(main_context.event_queue, al_get_display_event_source(main_context.display));
    
    //  font
    char * font = hashtable_search(config, "font");

    if (font)
        graphics_load_font(font);

    return 0;

}

void graphics_destroy(void)
{

    al_destroy_display(main_context.display);

    al_destroy_event_queue(main_context.event_queue);

    al_destroy_font(main_context.font[0]);
    al_destroy_font(main_context.font[1]);
    
    ttimer_delete(main_context.timer);

}

dimension_t graphics_get_screen_size(void)
{

    return SizeMake(al_get_display_width(main_context.display), al_get_display_height(main_context.display));

}

void graphics_load_font(char * path)
{

    ALLEGRO_BITMAP * font = al_load_bitmap(path);

    if (!font)
        return;
    
    dimension_t font_size = SizeMake(al_get_bitmap_width(font), al_get_bitmap_height(font));
    dimension_t big_font_size = SizeMultiplyBySize(font_size, SizeMake(2., 2.));
    
    //  ingrandimento del font normale
    ALLEGRO_BITMAP * font_big = al_create_bitmap(big_font_size.width, big_font_size.height);
    al_set_target_bitmap(font_big);
    al_draw_scaled_bitmap(font, 0, 0, font_size.width, font_size.height, 0, 0, big_font_size.width, big_font_size.height, 0);
    al_set_target_backbuffer(main_context.display);
    
    al_convert_mask_to_alpha(font, al_map_rgb(0, 0, 0));
    al_convert_mask_to_alpha(font_big, al_map_rgb(0, 0, 0));

    debugf("[Font] %s caricato\n", path);

    int ranges[] = {
        0x0020, 0x007F
    };

    main_context.font[0] = al_grab_font_from_bitmap(font, sizeof(ranges) / (sizeof(int) * 2), ranges);
    main_context.font[1] = al_grab_font_from_bitmap(font_big, sizeof(ranges) / (sizeof(int) * 2), ranges);
    
}

void graphics_fonts_swap(void) {
    
    ALLEGRO_FONT * f = main_context.font[0];
    main_context.font[0] = main_context.font[1];
    main_context.font[1] = f;
    
}
