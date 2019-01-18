#ifndef main_image_h
#define main_image_h

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>

#include "misc/geometry.h"

#include "main/color.h"

/**
 *  Immagine (bitmap)
 */
typedef struct image_s {

    /** Immagine della liberia grafica */
    ALLEGRO_BITMAP * bitmap;

    /** Dimensioni in pixel */
    dimension_t size;

    /** Area dell'bitmap da considerare, nel caso si tratti di una bitmap composta da un'insieme di immagini */
    rectangle_t rectangle;

} image_t;

/**
 *  Crea una nuova immagine duplicandone un'altra e considerandone una regione
 *
 *  @param image Immagine da copiare
 *  @param region Regione da copiare
 *
 *  @return Nuova immagine
 */
image_t * image_new(image_t * image, rectangle_t region);

/**
 *  Crea una nuova immagine caricandola dal disco e considerandone una regione
 *
 *  @param path Path dell'immagine
 *  @param region Regione da copiare
 *  @param mask_alpha Se convertire o meno il colore #007575 in trasparenza
 *
 *  @return Nuova immagine
 */
image_t * image_load_new(char * path, rectangle_t region, bool mask_alpha);

/**
 *  Crea una copia 1:1 di un'immagine
 *
 *  @param image Immagine da copiare
 *
 *  @return Copia
 */
image_t * image_duplicate(image_t * image);

/**
 *  Crea una copia 1:1 di un'immagine e applica una tinta
 *
 *  @param image Immagine da copiare
 *  @param color Colore della tinta
 *
 *  @return Copia
 */
image_t * image_duplicate_tinted(image_t * image, color_t color);

/**
 *  Crea una nuova immagine riempiendola di un certo colore
 *
 *  @param fill Colore di riempimento
 *  @param size Dimensione
 *
 *  @return Immagine
 */
image_t * image_create_new(color_t fill, dimension_t size);

/**
 *  Dealloca un'immagine
 *
 *  @param image Immagine da deallocare
 */
void image_delete(image_t * image);

/**
 *  Interpreta un certo colore dell'immagine come trasparente
 *
 *  @param image Immagine
 *  @param color Colore
 */
void image_mask_alpha(image_t * image, color_t color);


#endif
