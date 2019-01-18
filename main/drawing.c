#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>

#include "main/graphics.h"
#include "main/drawing.h"

void rectangle_draw(rectangle_t rect, color_t color)
{

    al_draw_filled_rectangle(
        rect.origin.x,
        rect.origin.y,
        rect.origin.x + rect.size.width,
        rect.origin.y + rect.size.height,
        color_to_al(color)
    );

}

void rectangle_draw_gradient(rectangle_t rect, color_t color_a, color_t color_b, bool horizontal)
{

    if (horizontal) {
        
        ALLEGRO_VERTEX v[] = {
            { rect.origin.x, rect.origin.y,                                     0, 0, 0, color_to_al(color_b) }, //top left
            { rect.origin.x + rect.size.width, rect.origin.y,                   0, 0, 0, color_to_al(color_b) }, //top right
            { rect.origin.x, rect.origin.y + rect.size.width,                   0, 0, 0, color_to_al(color_a) }, //bottom left
            { rect.origin.x + rect.size.width, rect.origin.y + rect.size.width, 0, 0, 0, color_to_al(color_a) }  //bottom right
        };
        
        al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
        
    } else {
        
        ALLEGRO_VERTEX v[] = {
            { rect.origin.x, rect.origin.y,                                     0, 0, 0, color_to_al(color_a) }, //top left
            { rect.origin.x + rect.size.width, rect.origin.y,                   0, 0, 0, color_to_al(color_b) }, //top right
            { rect.origin.x, rect.origin.y + rect.size.width,                   0, 0, 0, color_to_al(color_a) }, //bottom left
            { rect.origin.x + rect.size.width, rect.origin.y + rect.size.width, 0, 0, 0, color_to_al(color_b) }  //bottom right
        };
        
        al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
        
    }


}

