#include <math.h>

#include "game/cell.h"
#include "game/events.h"
#include "game/map.h"
#include "game/powerup.h"

#include "misc/geometry.h"
#include "misc/random.h"

#include "main/drawing.h"
#include "main/graphics.h"
#include "main/output.h"
#include "main/tiling.h"

void output_image_opacity(image_t * image, point_t location, unsigned char opacity)
{

    //  disegna l'immagine
    al_draw_tinted_bitmap_region(
        image->bitmap,                            //  bitmap
        al_map_rgba(255, 255, 255, opacity),      //  opacità
        image->rectangle.origin.x,                //  X: sorgente
        image->rectangle.origin.y,                //  Y: sorgente
        image->rectangle.size.width,              //  W: sorgente
        image->rectangle.size.height,             //  H: sorgente
        location.x,                               //  X: destinazione
        location.y,                               //  Y: destinazione
        0
    );

}

void output_image_tile_tinted(image_t * image, rectangle_t source, point_t location, color_t color)
{

    //  disegna l'immagine
    al_draw_tinted_bitmap_region(
        image->bitmap,        //  bitmap
        color_to_al(color),   //  colore
        source.origin.x,      //  X: sorgente
        source.origin.y,      //  Y: sorgente
        source.size.width,    //  W: sorgente
        source.size.height,   //  H: sorgente
        location.x,           //  X: destinazione
        location.y,           //  Y: destinazione
        0
    );

}

void output_image(image_t * image, point_t location)
{

    //  disegna l'immagine
    al_draw_bitmap_region(
        image->bitmap,                //  bitmap
        image->rectangle.origin.x,    //  X: sorgente
        image->rectangle.origin.y,    //  Y: sorgente
        image->rectangle.size.width,  //  W: sorgente
        image->rectangle.size.height, //  H: sorgente
        location.x,                   //  X: destinazione
        location.y,                   //  Y: destinazione
        0
    );

}

void output_image_tile(image_t * image, rectangle_t source, point_t location)
{

    //  disegna l'immagine
    al_draw_bitmap_region(
        image->bitmap,        //  bitmap
        source.origin.x,      //  X: sorgente
        source.origin.y,      //  Y: sorgente
        source.size.width,    //  W: sorgente
        source.size.height,   //  H: sorgente
        location.x,           //  X: destinazione
        location.y,           //  Y: destinazione
        0
    );

}

void output_screen_redraw(void)
{

    al_flip_display();
    al_clear_to_color(color_to_al(ColorBackground));

}

void output_handle_screen_redraw(game_t * game, event_t event) {
    
    output_screen_redraw();
    
}

void output_screen_fill(color_t color)
{
    
    al_clear_to_color(color_to_al(color));
    
}

void output_set_origin(point_t origin)
{

    //  applica una traslazione a tutti i contenuti in modo tale da centrare la mappa senza troppe difficoltà
    ALLEGRO_TRANSFORM t;
    al_identity_transform(&t);
    al_translate_transform(&t, origin.x, origin.y);
    al_use_transform(&t);

    main_context.origin = origin;

}

void output_set_scale(dimension_t scale)
{

    ALLEGRO_TRANSFORM ts;
    al_identity_transform(&ts);
    al_scale_transform(&ts, scale.width, scale.height);
    al_use_transform(&ts);

}

void output_stringf(color_t color, point_t location, char * fmt, ...)
{
    
    if (!main_context.font[0])
        return;
    
    va_list ap;

    //  inizializzazione della lista dei parametri
    va_start(ap, fmt);

    //  calcolo lunghezza della stringa "formattata"
    size_t size_to_alloc = vsnprintf(NULL, 0, fmt, ap);

    //  c'è qualche errore
    if (!size_to_alloc) {
        va_end(ap);
        return;
    }

    //  re-inizializzazione della lista
    va_start(ap, fmt);

    //  \0
    size_to_alloc++;

    //  allocazione dello spazio necessario
    char * buffer = memalloc(char, size_to_alloc, 1);

    //  creazione stringa formattata
    vsprintf(buffer, fmt, ap);

    al_draw_text(
        main_context.font[0],
        color_to_al(color),
        location.x,
        location.y,
        ALLEGRO_ALIGN_LEFT,
        buffer
    );

    //  deallocazione della memoria
    memfree(buffer);

    va_end(ap);

}

dimension_t output_get_string_size(char * string)
{
    
    if (string == NULL)
        string = "";
    
    if (!main_context.font[0])
        return SizeZero;
    
    int width;
    int height;

    al_get_text_dimensions(main_context.font[0], string, NULL, NULL, &width, &height);

    return SizeMake(width, height);

}

void output_fullscreen_message(char * title, char * message, color_t color)
{

    if (!main_context.font[0])
        return;
    
    dimension_t screen = graphics_get_screen_size();
    
    //  font grande
    graphics_fonts_swap();
    
    point_t origin = PointZero;
    dimension_t scale = SizeMake(2., 2.);
    
    //  tutto quello che viene disegnato è ingrandito di 2 volte
    output_set_scale(scale);

    //  calcolo della dimensione della prima stringa
    dimension_t size = output_get_string_size(title);
    
    //  coordinate del testo
    origin.x = (screen.width / scale.width - size.width) / 2.;
    origin.y = (screen.height / scale.height - size.height) / 2.;

    //  prima stringa
    output_stringf(color, origin, title);

    //  posizione iniziale della seconda stringa
    origin.y += size.height + 10.;
    
    //  ripristino font normale
    graphics_fonts_swap();
    
    //  calcolo della dimensione della seconda stringa
    size = output_get_string_size(message);
    origin.x = (screen.width / scale.width - size.width) / 2.;
    
    //  seconda stringa
    output_stringf(ColorText1, origin, message);

}

/**
 *  Effettua il tiling e disegna una cella della mappa
 *
 *  @param level Livello di appartenenenza
 *  @param map Mappa
 *  @param point Coordinate della cella
 */
void output_map_cell(level_t * level, map_t * map, point_t point)
{
    
    point_t tile_offset[4] = {
        PointMake( 0,  0),
        PointMake(16,  0),
        PointMake(0,  16),
        PointMake(16, 16)
    };
    
    point_t points[4];

    //  calcola i tiles da utilizzare
    map_get_tiling_points(map, point, points);

    //  posizione alla quale disegnare
    point_t position = cell_location_to_position(point);

    //  cella di destinazione
    map_cell_t * cell = map_get_cell(map, point);
    
    //  controlla se il bonus è scaduto
    if (cell->powerup.powerup)
        powerup_check_cell(cell);
    
    //  immagine da disegnare, diversa a seconda del valore della cella
    image_t * texture;

    if (cell_is_path(cell))
        texture = level->textures[LEVEL_TEXTURE_PATH_LIGHTEST + ((int)cell_get_value(cell) - 1)];
    else
        texture = level->textures[LEVEL_TEXTURE_WALL];

    int i;

    //  disegna la cella 1/4 per volta
    for (i = 0; i < 4; i++) {
        
        rectangle_t source;
        source.size = BackgroundTileSize;
        source.origin = PointPointOffset(texture->rectangle.origin, points[i]);

        output_image_tile(texture, source, PointPointOffset(position, tile_offset[i]));

        //  se c'è un bonus disegna anche quello
        if (cell->powerup.powerup)
        {
            powerup_t * powerup = cell->powerup.powerup;
            
            point_t off;
            
            off.x = (CellSize.width - powerup->tile->size.width) / 2.;
            off.y = (CellSize.height - powerup->tile->size.height) / 2.;

            output_image(powerup->tile, PointPointOffset(position, off));
        }
    }


}

void output_map(level_t * level, map_t * map)
{

    //  la bitmap viene disegnata in differita per ottimizzare le prestazioni
    al_hold_bitmap_drawing(true);

    int y = 0, x = 0;

    //  disegno
    for (y = 0; y < map->size.height; y++) {
        for (x = 0; x < map->size.width; x++) {
            output_map_cell(level, map, PointMake(x, y));
        }
    }

    al_hold_bitmap_drawing(false);

    //  l'uscita è colorata in modo leggermente diverso
    rectangle_t r;
    r.origin = cell_location_to_position(map->end);
    r.size = CellSize;
    rectangle_draw_gradient(r, ColorMakeRGBA(0, 0, 0, 255), ColorMakeRGBA(0, 0, 0, 0), ((int)map->end.y == 0));

}

