#ifndef game_structs_h
#define game_structs_h

#include "types.h"

typedef struct animation_s animation_t;
TYPE_FUNCTIONS_DECLARE(animation);

typedef struct character_s character_t;
TYPE_FUNCTIONS_DECLARE(character);

typedef struct event_s event_t;

typedef struct game_s game_t;

typedef struct level_s level_t;
TYPE_FUNCTIONS_DECLARE(level);

typedef struct map_s map_t;
TYPE_FUNCTIONS_DECLARE(map);

typedef struct map_cell_s map_cell_t;

typedef struct powerup_s powerup_t;
TYPE_FUNCTIONS_DECLARE(powerup);

typedef struct powerup_location_s   powerup_location_t;
typedef struct powerup_status_s     powerup_status_t;
TYPE_FUNCTIONS_DECLARE(powerup_status);

#endif
