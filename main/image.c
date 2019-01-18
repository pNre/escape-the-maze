#include <errno.h>
#include <allegro5/allegro_primitives.h>

#include "utils.h"

#include "main/drawing.h"
#include "main/fs.h"
#include "main/graphics.h"
#include "main/image.h"

image_t * image_new(image_t * source, rectangle_t region)
{

    //  creazione struttura
    image_t * image = memalloc(image_t);

    //  immagine
    //image->bitmap = source->bitmap;
    image->bitmap = al_clone_bitmap(source->bitmap);

    //  dimensioni
    image->size = region.size;

    //  regione
    image->rectangle = region;

    return image;

}

image_t * image_load_new(char * path, rectangle_t region, bool mask_alpha)
{

    //  percorso assoluto
    char * absolute_path = fs_construct_path(fs_get_resources_path(), path);
    
    //  caricamento immagine
    ALLEGRO_BITMAP * bitmap = al_load_bitmap(absolute_path);

    //  problemi di caricamento
    if (!bitmap) {
        debugf("[Bitmap] %s, error: %d\n", absolute_path, errno);
        memfree(absolute_path);
        return NULL;
    }

    debugf("[Bitmap] %s caricata\n", absolute_path);
    
    memfree(absolute_path);

    //  creazione struttura
    image_t * image = memalloc(image_t);

    //  immagine
    image->bitmap = bitmap;

    //  dimensioni
    image->size = SizeMake(al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap));

    //  regione
    if (RectIsNull(region))
        image->rectangle = RectMake(0, 0, image->size.width, image->size.height);
    else {
        image->rectangle = region;
        image->size = region.size;
    }
    
    //  trasparenza
    if (mask_alpha)
        image_mask_alpha(image, ColorAlpha);

    return image;

}

image_t * image_create_new(color_t fill, dimension_t size)
{

    //  creazione struttura
    image_t * image = memalloc(image_t);

    //  immagine
    image->bitmap = al_create_bitmap(size.width, size.height);

    if (!image->bitmap) {
        memfree(image);
        return NULL;
    }
    
    //  riempimento dell'immagine
    al_set_target_bitmap(image->bitmap);
    al_draw_filled_rectangle(0, 0, size.width, size.height, al_map_rgba(fill.r, fill.g, fill.b, fill.a));
    al_set_target_backbuffer(main_context.display);

    //  dimensioni
    image->size = size;

    //  regione
    image->rectangle = RectMake(0, 0, size.width, size.height);

    return image;

}

image_t * image_duplicate(image_t * image)
{

    if (!image)
        return NULL;
    
    image_t * dup = memalloc(image_t);

    dup->size = image->size;
    dup->rectangle = image->rectangle;

    dup->bitmap = al_clone_bitmap(image->bitmap);

    if (!dup->bitmap) {
        memfree(dup);
        dup = NULL;
    }
    
    return dup;

}

image_t * image_duplicate_tinted(image_t * image, color_t color)
{
    
    if (!image)
        return NULL;

    image_t * dup = image_duplicate(image);

    if (!dup)
        return NULL;
    
    al_set_target_bitmap(dup->bitmap);

    rectangle_draw(dup->rectangle, color);

    al_set_target_backbuffer(main_context.display);

    return dup;

}

void image_mask_alpha(image_t * image, color_t color)
{

    if (!image || !image->bitmap)
        return;

    al_convert_mask_to_alpha(image->bitmap, al_map_rgb(color.r, color.g, color.b));

}

void image_delete(image_t * image)
{
    
    if (!image)
        return;

    //  1. deallocazione bitmap
    if (image->bitmap)
        al_destroy_bitmap(image->bitmap);

    //  2. deallocazione image_t
    memfree(image);

}
