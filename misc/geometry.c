#include <stdio.h>

#include "types.h"

#include "misc/geometry.h"

rectangle_t rectangle_centered_make(point_t center, dimension_t rect_size, dimension_t max_size)
{

    //  il centro del rettangolo deve corrispondere con la posizione del personaggio
    float x = center.x - (rect_size.width / 2.);
    float y = center.y - (rect_size.height / 2.);

    rectangle_t rectangle;

    rectangle.origin.x = x >= 0. ? x : 0.;
    rectangle.origin.y = y >= 0. ? y : 0.;

    //  se il rettangolo supera gli estremi viene limitato
    if (x + rect_size.width >= max_size.width)
        rectangle.size.width = max_size.width - x;
    else
        rectangle.size.width = rect_size.width;

    if (y + rect_size.height >= max_size.height)
        rectangle.size.height = max_size.height - y;
    else
        rectangle.size.height = rect_size.height;

    return rectangle;

}

float rectangle_intersection_size(rectangle_t rect_a, rectangle_t rect_b)
{

    float x = fmax(0, fminf(RectGetMaxX(rect_a), RectGetMaxX(rect_b)) - fmax(RectGetMinX(rect_a), RectGetMinX(rect_b)));
    float y = fmax(0, fminf(RectGetMaxY(rect_a), RectGetMaxY(rect_b)) - fmax(RectGetMinY(rect_a), RectGetMinY(rect_b)));

    float area = RectArea(rect_a) + RectArea(rect_b);
    float intersection = (x * y);

    return intersection / (area - intersection);

}

point_t rectangle_closest_corner_to_point(rectangle_t rect, point_t point)
{

    //  angoli del riquadro
    const point_t points[] = {
        PointMake(RectGetMinX(rect), RectGetMinY(rect)),
        PointMake(RectGetMaxX(rect), RectGetMinY(rect)),
        PointMake(RectGetMinX(rect), RectGetMinY(rect)),
        PointMake(RectGetMaxX(rect), RectGetMaxY(rect))
    };

    //  angolo più vicino a point
    point_t closest = points[0];
    
    unsigned int i;
    for (i = 1; i < array_count(points); i++) {
        
        if (Manhattan(closest, point) > Manhattan(points[i], point))
            closest = points[i];
        
    }

    return closest;

}

void * duplicate_dimensions(void * dim)
{

    if (!dim)
        return NULL;

    dimension_t * dup = memalloc(dimension_t);

    *dup = *((dimension_t *)dim);

    return dup;

}

void delete_dimensions(void * dim)
{

    memfree(dim);

}

size_t sizeof_dimensions(void * dim)
{
    return sizeof(dimension_t);
}

void * duplicate_rectangle(void * rect)
{

    if (!rect)
        return NULL;

    rectangle_t * dup = memalloc(rectangle_t);

    dup->origin = ((rectangle_t *)rect)->origin;
    dup->size = ((rectangle_t *)rect)->size;

    return dup;

}

void delete_rectangle(void * rect)
{

    memfree(rect);

}

size_t sizeof_rectangle(void * point)
{
    return sizeof(rectangle_t);
}

TYPE_FUNCTIONS_DEFINE(dimension, delete_dimensions, sizeof_dimensions, duplicate_dimensions);
TYPE_FUNCTIONS_DEFINE(rectangle, delete_rectangle, sizeof_rectangle, duplicate_rectangle);

