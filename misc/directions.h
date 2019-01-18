#ifndef misc_directions_h
#define misc_directions_h

/**
 *  Definisce le possibili direzioni che un personaggio può tenere
 */
enum {
    DIRECTION_NONE = -1,
    DIRECTION_NORTH,
    DIRECTION_EAST,
    DIRECTION_SOUTH,
    DIRECTION_WEST
};

/**
 *  Calcola la direzione opposta rispetto ad una direzione
 *
 *  @param direction Direzione della quale calcolare l'opposta
 *
 *  @return Direzione opposta
 */
sinline int direction_opposite(int direction)
{
    switch (direction) {
        case DIRECTION_EAST:
            return DIRECTION_WEST;

        case DIRECTION_SOUTH:
            return DIRECTION_NORTH;

        case DIRECTION_WEST:
            return DIRECTION_EAST;

        case DIRECTION_NORTH:
            return DIRECTION_SOUTH;
    }

    return DIRECTION_NONE;
}

#endif
