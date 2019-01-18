#include "game/map.h"
#include "game/character.h"
#include "game/powerup.h"

#include "main/tiling.h"

#include "misc/geometry.h"

/** Offset dei tasselli all'interno della bitmap dello sfondo */
static point_t tiles_specs[4][32] = {
    [TILE_A][TILE_PART_NONE]                            = PointMake(32, 64),
    [TILE_A][TILE_PART_TOP]                             = PointMake(32, 32),
    [TILE_A][TILE_PART_LEFT]                            = PointMake(0,  64),
    [TILE_A][TILE_PART_TOP | TILE_PART_LEFT]            = PointMake(0,  32),
    [TILE_A][TILE_PART_ANGLE]                           = PointMake(32, 0),
    
    [TILE_B][TILE_PART_NONE]                            = PointMake(16, 64),
    [TILE_B][TILE_PART_TOP]                             = PointMake(16, 32),
    [TILE_B][TILE_PART_RIGHT]                           = PointMake(48, 64),
    [TILE_B][TILE_PART_TOP | TILE_PART_RIGHT]           = PointMake(48, 32),
    [TILE_B][TILE_PART_ANGLE]                           = PointMake(48, 0),
    
    [TILE_C][TILE_PART_NONE]                            = PointMake(32, 48),
    [TILE_C][TILE_PART_LEFT]                            = PointMake(0,  48),
    [TILE_C][TILE_PART_BOTTOM]                          = PointMake(32, 80),
    [TILE_C][TILE_PART_BOTTOM | TILE_PART_LEFT]         = PointMake(0,  80),
    [TILE_C][TILE_PART_ANGLE]                           = PointMake(32, 16),
    
    [TILE_D][TILE_PART_NONE]                            = PointMake(16, 48),
    [TILE_D][TILE_PART_BOTTOM]                          = PointMake(16, 80),
    [TILE_D][TILE_PART_RIGHT]                           = PointMake(48, 48),
    [TILE_D][TILE_PART_BOTTOM | TILE_PART_RIGHT]        = PointMake(48, 80),
    [TILE_D][TILE_PART_ANGLE]                           = PointMake(48, 16),
};

/**
 *  Se presente, ritorna il tipo della cella ad un certo punto della mappa
 *
 *  @param map Mappa
 *  @param point Punto della mappa
 */
int map_tiling_get_tile_type(map_t * map, point_t point)
{

    //  point è sul bordo sinistro o destro
    if (point.x < 0 || point.x >= map->size.width)
        return CELL_TYPE_UNKNOWN;

    //  point è sul bordo in alto o in basso
    if (point.y < 0 || point.y >= map->size.height)
        return CELL_TYPE_UNKNOWN;

    map_cell_t * cell = map_get_cell(map, point);
    
    return cell->type;

}

void map_get_tiling_points(map_t * map, point_t point, point_t * points)
{

    map_cell_t * cell = map_get_cell(map, point);

    unsigned char a = 0, b = 0, c = 0, d = 0;
    int cell_type = cell->type;
    
    int top     = map_tiling_get_tile_type(map, PointPointOffset(point, OffsetTop));
    int left    = map_tiling_get_tile_type(map, PointPointOffset(point, OffsetLeft));
    int right   = map_tiling_get_tile_type(map, PointPointOffset(point, OffsetRight));
    int bottom  = map_tiling_get_tile_type(map, PointPointOffset(point, OffsetBottom));

    if (top != cell_type) {
        a |= TILE_PART_TOP;
        b |= TILE_PART_TOP;
    }

    if (right != cell_type) {
        b |= TILE_PART_RIGHT;
        d |= TILE_PART_RIGHT;
    }

    if (left != cell_type) {
        a |= TILE_PART_LEFT;
        c |= TILE_PART_LEFT;
    }

    if (bottom != cell_type) {
        c |= TILE_PART_BOTTOM;
        d |= TILE_PART_BOTTOM;
    }

    if (top == cell_type &&
        left == cell_type &&
        map_tiling_get_tile_type(map, PointPointOffset(point, OffsetTopLeft)) != cell_type)
        a = TILE_PART_ANGLE;

    if (top == cell_type &&
        right == cell_type &&
        map_tiling_get_tile_type(map, PointPointOffset(point, OffsetTopRight)) != cell_type)
        b = TILE_PART_ANGLE;

    if (left == cell_type &&
        bottom == cell_type &&
        map_tiling_get_tile_type(map, PointPointOffset(point, OffsetBottomLeft)) != cell_type)
        c = TILE_PART_ANGLE;

    if (right == cell_type &&
        bottom == cell_type &&
        map_tiling_get_tile_type(map, PointPointOffset(point, OffsetBottomRight)) != cell_type)
        d = TILE_PART_ANGLE;

    points[TILE_A] = tiles_specs[TILE_A][a];
    points[TILE_B] = tiles_specs[TILE_B][b];
    points[TILE_C] = tiles_specs[TILE_C][c];
    points[TILE_D] = tiles_specs[TILE_D][d];

}

void character_tiling(character_t * character, image_t * texture, rectangle_t rect)
{

    //  tiling
    //  la bitmap è divisa in 4 righe e 3 colonne
    //  riga 1: personaggio rivolto verso l'alto
    //  riga 2: verso destra
    //  riga 3: verso il basso
    //  riga 4: verso sinistra
    int y, x;
    int i = 0;

    for (y = 0; y < rect.size.height; y += CharacterTileSize.height) {
        for (x = 0; x < rect.size.width; x += CharacterTileSize.width) {
            character->tiles[i++] = image_new(texture, RectMake(rect.origin.x + x, rect.origin.y + y, CharacterTileSize.width, CharacterTileSize.height));
        }
    }

}
