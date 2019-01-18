#include <stdio.h>

#include "std/stack.h"

#include "misc/geometry.h"

#include "game/map.h"

#include "main/drawing.h"

bool shortest_path_find(map_t * map, map_cell_t * a, map_cell_t * b, sstack_t * path)
{

    if (!cell_get_parent(b)) {

        return false;

    } else if (a != b) {

        stack_push(path, b);
        return shortest_path_find(map, a, cell_get_parent(b), path);

    }

    return true;

}

void path_find(map_t * map, map_cell_t * b, sstack_t * path)
{

    if (b) {

        stack_push(path, b);
        path_find(map, cell_get_parent(b), path);

    }

}

void path_draw(sstack_t * path, color_t color)
{

    foreach(path, map_cell_t *, cell) {

        point_t p = cell_location_to_position(cell->location);
        dimension_t rect_size = SizeMake(16, 16);
        rectangle_t rect = RectZero;

        rect.origin.x = p.x + (CellSize.width - rect_size.width) / 2;
        rect.origin.y = p.y + (CellSize.height - rect_size.height) / 2;
        rect.size = rect_size;

        rectangle_draw(rect, color);

    }

}