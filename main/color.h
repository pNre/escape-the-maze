#ifndef main_color_h
#define main_color_h

#include "misc/random.h"

/**
 *  Colore RGBA
 */
typedef struct color_s {

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

} color_t;

#define color_to_al(color) al_map_rgba(color.r, color.g, color.b, 255 - color.a)

/**
 *  Inizializza un colore date le componenti RGBA
 *
 *  @param r Red
 *  @param g Green
 *  @param b Blue
 *  @param a Alpha
 */
#define ColorMakeRGBA(r, g, b, a) ((color_t){r, g, b, a})

/**
 *  Inizializza un colore date le componenti RGB
 *
 *  @param r Red
 *  @param g Green
 *  @param b Blue
 */
#define ColorMakeRGB(r, g, b)   ColorMakeRGBA(r, g, b, 0)

/** Alcuni colori di default */

#define ColorRandom(base)   ColorMakeRGB(   \
    random_int(0, (255 - base)) + base,     \
    random_int(0, (255 - base)) + base,     \
    random_int(0, (255 - base)) + base)

#define ColorBlack      ColorMakeRGB(0, 0, 0)

#define ColorBackground ColorMakeRGB(38, 50, 72)

#define ColorArea       ColorMakeRGB(223, 235, 255)

#define ColorText1      ColorMakeRGB(147, 161, 189)
#define ColorText2      ColorMakeRGB(255, 83, 13)

#define ColorAlpha      ColorMakeRGB(0, 0x75, 0x75)

#endif
