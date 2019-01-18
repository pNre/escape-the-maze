#ifndef main_drawing_drawing_h
#define main_drawing_drawing_h

#include "types.h"

#include "main/color.h"

#include "misc/geometry.h"

/**
 *  Disegna un rettangolo
 *
 *  @param rect Coordinate e dimensioni del rettangolo
 *  @param color Colore
 */
void rectangle_draw(rectangle_t rect, color_t color);

/**
 *  Disegna un rettangolo con un gradiente
 *
 *  @param rect Coordinate e dimensioni del rettangolo
 *  @param color_a Colore inziale
 *  @param color_b Colore finale
 *  @param horizontal Direzione gradiente
 */
void rectangle_draw_gradient(rectangle_t rect, color_t color_a, color_t color_b, bool horizontal);

#endif
