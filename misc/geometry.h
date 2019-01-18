#ifndef misc_geometry_h
#define misc_geometry_h

#include <math.h>

#include "types.h"
#include "utils.h"

/**
 *  Coordinate di un punto dello schermo: (x, y)
 */
typedef struct point_s {

    float x, y;

} point_t;

/**
 *  Dimensioni di un oggetto: larghezza, altezza
 */
typedef struct dimension_s {

    float width, height;

} dimension_t;

/**
 *  Rettangolo
 */
typedef struct {

    /** Posizione */
    point_t origin;

    /** Dimensione */
    dimension_t size;

} rectangle_t;

/**
 *  Inizializza un punto date le sue coordinate
 */
#define PointMake(x, y)         ((point_t){(float)x, (float)y})

/** Punto (0, 0) */
#define PointZero               PointMake(0, 0)
/** Punto nullo (+inf, +inf) */
#define PointNull               PointMake(INFINITY, INFINITY)

/** Moltiplica le coordinate di un punto per un certo fattore */
#define PointMultiply(p, fact)  PointMake(p.x * fact, p.y * fact)

/** Calcola la traslazione di un punto, (pt.x + xf, pt.y + yf) */
#define PointOffset(pt, xf, yf)   PointMake(pt.x + xf, pt.y + yf)

/** Calcola la somma tra le coordinate di due punti */
#define PointPointOffset(pt, po)  PointOffset(pt, po.x, po.y)

/** Verifica che un punto sia nullo */
#define PointIsNull(point)      (point.x >= INFINITY && point.y >= INFINITY)

/** Confronta due punti */
#define PointEqualToPoint(a, b) (a.x == b.x && a.y == b.y)

#define OffsetTop               PointMake( 0., -1.)
#define OffsetTopLeft           PointMake(-1., -1.)
#define OffsetTopRight          PointMake( 1., -1.)
#define OffsetBottom            PointMake( 0.,  1.)
#define OffsetBottomLeft        PointMake(-1.,  1.)
#define OffsetBottomRight       PointMake( 1.,  1.)
#define OffsetLeft              PointMake(-1.,  0.)
#define OffsetRight             PointMake( 1.,  0.)

/**
 *  Inizializza delle dimensioni
 */
#define SizeMake(w, h)          ((dimension_t){(float)w, (float)h})

/** Dimensioni (0, 0) */
#define SizeZero                SizeMake(0, 0)

/** Dimensioni (+inf, +inf) */
#define SizeInfinity            SizeMake(INFINITY, INFINITY)

/** Verifica che un punto sia nullo */
#define SizeIsInfinity(size)    (size.width >= INFINITY && size.height >= INFINITY)

/** Verifica che le dimensioni siano entrambe 0 */
#define SizeIsZero(size)        (size.width == 0 && size.height == 0)

/** Moltiplica due dimensioni */
#define SizeMultiplyBySize(s1, s2)  SizeMake(s1.width * s2.width, s1.height * s2.height)

/** Accede al valore di una dimensione contenuto ad una certa locazione di memoria */
#define dimension_value(size)                  (size ? *((dimension_t *)size) : SizeZero)
#define dimension_value_nocheck(size)          (*((dimension_t *)size))

/**
 *  Inizializza un rettangolo date coordinate e dimensioni
 */
#define RectMake(x, y, w, h)    ((rectangle_t){{x, y}, {w, h}})

/** Rettangolo di origine (0, 0) e dimensione (0, 0) */
#define RectZero                RectMake(0, 0, 0, 0)

/** Rettangolo di origine nulla e dimensione (0, 0) */
#define RectNull                RectMake(INFINITY, INFINITY, 0, 0)

/** Verifica che un rettangolo sia nullo */
#define RectIsNull(rect)        (PointIsNull(rect.origin) && SizeIsZero(rect.size))

/** Coordinata X dell'origine di un rettangolo */
#define RectGetMinX(rect)       (rect.origin.x)
/** Coordinata Y dell'origine di un rettangolo */
#define RectGetMinY(rect)       (rect.origin.y)
/** Coordinata X dell'origine di un rettangolo + larghezza del rettangolo */
#define RectGetMaxX(rect)       (rect.origin.x + rect.size.width)
/** Coordinata Y dell'origine di un rettangolo + altezza del rettangolo */
#define RectGetMaxY(rect)       (rect.origin.y + rect.size.height)

/** Verifica se due rettangoli si intersecano */
#define RectIntersectsRect(a, b)    !(RectGetMinX(a) > RectGetMaxX(b) ||    \
                                      RectGetMaxX(a) < RectGetMinX(b) ||    \
                                      RectGetMinY(a) > RectGetMaxY(b) ||    \
                                      RectGetMaxY(a) < RectGetMinY(b))

/** Calcola l'area di un rettangolo */
#define RectArea(a)             (a.size.width * a.size.height)

/** Verifica che un rettangolo contenga al suo interno un punto */
#define RectContainsPoint(rect, point)  (point.x >= RectGetMinX(rect) &&    \
                                         point.y >= RectGetMinY(rect) &&    \
                                         point.x <= RectGetMaxX(rect) &&    \
                                         point.y <= RectGetMaxY(rect))

/** Verifica che un rettangolo contenga al suo interno un punto */
#define RectContainsPointStrict(rect, point)  (point.x > RectGetMinX(rect) &&   \
                                               point.y > RectGetMinY(rect) &&   \
                                               point.x < RectGetMaxX(rect) &&   \
                                               point.y < RectGetMaxY(rect))

/** Verifica che un rettangolo (a) contenga al suo interno un altro rettangolo (b) */
#define RectContainsRect(a, b)      (RectGetMinX(b) >= RectGetMinX(a) &&    \
                                     RectGetMaxX(b) <= RectGetMaxX(a) &&    \
                                     RectGetMinY(b) >= RectGetMinY(a) &&    \
                                     RectGetMaxY(b) <= RectGetMaxY(a))

/** Verifica che un rettangolo (a) sia uguale ad un altro rettangolo (b) */
#define RectEqualToRect(a, b)       (RectGetMinX(a) == RectGetMinX(b) &&    \
                                     RectGetMaxX(a) == RectGetMaxX(b) &&    \
                                     RectGetMinY(a) == RectGetMinY(b) &&    \
                                     RectGetMaxY(a) == RectGetMaxY(b))

/** Accede al valore di un rettangolo contenuto ad una certa locazione di memoria */
#define rectangle_value(rect)           (rect ? *((rectangle_t *)rect) : RectZero)
#define rectangle_value_nocheck(rect)   (*((rectangle_t *)rect))

/** Tipo di una funzione per il calcolo della distanza tra due punti */
typedef float (* distance_function)(point_t, point_t);

/**
 *  Calcolo l'angolo, di un rettangolo, che si trova più vicino ad un punto
 *
 *  @param rect Rettangolo
 *  @param point Punto
 *
 *  @return Angolo più vicino al punto
 */
point_t rectangle_closest_corner_to_point(rectangle_t rect, point_t point);

/**
 *  Calcola la percentuale di intersezione delle aree di due rettangoli
 *
 *  @param rect_a Rettangolo A
 *  @param rect_b Rettangolo B
 *
 *  @return Livello di intersezione (1. = intersezione completa, 0. nessuna intersezione)
 */
float rectangle_intersection_size(rectangle_t rect_a, rectangle_t rect_b);

/**
 *  Calcola un rettangolo centrato in un certo punto
 *
 *  @param center Centro del rettangolo
 *  @param rect_size Dimensione del rettangolo
 *  @param max_size Dimensione massima del rettangolo
 *
 *  @return Rettangolo
 */
rectangle_t rectangle_centered_make(point_t center, dimension_t rect_size, dimension_t max_size);

/**
 *  Calcola la distanza Manhattan tra due punti
 *  Distanza \f$(x_1,y_1)\f$, \f$(x_2,y_2) = |x_1-x_2|+|y_1-y_2|\f$
 *
 *  @param a Punto A
 *  @param b Punto B
 *
 *  @return Distanza tra i due punti
 */
sinline float Manhattan(point_t a, point_t b)
{
    return (fabs(a.x - b.x) + fabs(a.y - b.y));
}

TYPE_FUNCTIONS_DECLARE(dimension);
TYPE_FUNCTIONS_DECLARE(rectangle);

#endif
