#ifndef main_tiling_h
#define main_tiling_h

#include "game/structs.h"

//  I file grafici contenenti gli sfondi (siano essi del terreno o delle mura)
//  sono costituiti da immagini di dimensione 64x96 divisibile in 6 tasselli 32x32.
//  Ogni tassello è a sua volta divisibile in altri 4 tasselli 16x16.
//  Al momento di disegnare lo sfondo, ogni cella di sfondo va costruita utilizzando
//  4 dei tasselli 16x16 scegliendoli in base a quello che è il contenuto delle celle
//  vicine.

/** Definisce i tipi di tile,
 *  ogni tile può rappresentare un immagine con:
 *  - nessun bordo
 *  - bordo in alto
 *  - bordo a destra
 *  - bordo in basso
 *  - bordo a sinistra
 *  - angolo
 *  - una combinazione dei precedenti
 */
enum {

    TILE_PART_NONE          =   0,
    TILE_PART_TOP           =   1 << 0,
    TILE_PART_RIGHT         =   1 << 1,
    TILE_PART_BOTTOM        =   1 << 2,
    TILE_PART_LEFT          =   1 << 3,
    TILE_PART_ANGLE         =   1 << 4

};

/** Definisce i 4 tasselli che compongono una cella */
enum {

    TILE_A,
    TILE_B,
    TILE_C,
    TILE_D

};

/** Dimensione dei tiles dello sfondo */
#define BackgroundTileSize  SizeMake(16, 16)
/** Dimensione dei tiles dei personaggi */
#define CharacterTileSize   SizeMake(24, 32)

/**
 *  Data una mappa e un certo punto di essa, in points[4] inserisce le coordinate dei quattro tile 16x16 da disegnare
 *
 *  @param map Mappa
 *  @param point Punto della mappa
 *  @param points Array che conterrà la coordinate dei tiles
 */
void map_get_tiling_points(map_t * map, point_t point, point_t * points);

/**
 *  Dato un personaggio, effettua il tiling
 *
 *  @param character Personaggio
 *  @param texture Bitmap che contiene i tiles del personaggio
 *  @param rect Area della bitmap da considerare
 */
void character_tiling(character_t * character, image_t * texture, rectangle_t rect);

#endif
