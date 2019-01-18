#include "game/character.h"
#include "game/game.h"
#include "game/map.h"
#include "game/powerup.h"

#include "game/info.h"

void info_update(game_t * game)
{
    
    //  utente
    character_t * user = game_get_user(game);
    
    //  mappa nella quale si trova l'utente
    map_t * map = user->map;
    
    //  colore del testo
    color_t text_color = ColorText1;
    
    //  spaziatura verticale
    float spacing = 5;
    
    //  posizione iniziale
    point_t origin = PointMake(0, map->size.height * CellSize.height + spacing);
    
    //  legenda con informazioni sull'utente e sul gioco
    point_t point = origin;
    
    //  altezza di una stringa
    dimension_t string_size = output_get_string_size(NULL);
    
    int w = map->size.width * (CellSize.width / 2);
    int h = string_size.height;
    
    output_stringf(text_color, point, "Power-UPs");
    point.y += h;
    
    //  l'utente non ha bonus
    if (list_empty(user->powerups)) {
        
        output_stringf(text_color, point, "--------");
        point.y += h;
        
    } else {
        
        //  lista dei bonus
        foreach(user->powerups, powerup_status_t *, status) {
            
            //  il bonus è terminato, si passa al prossimo
            if (!status->counter || !status->powerup->trigger)
                continue;
            
            int trigger = status->powerup->trigger ? status->powerup->trigger : '/';
            long time_left = (status->powerup->duration - status->elapsed);
            
            output_stringf(text_color, point, "%c: %s (%d) - %lds", trigger, status->powerup->name, status->counter, time_left);
            point.y += h;
            
        }
    }
    
    //  statistiche
    point = PointMake(w, origin.y);
    
    //  vite
    if (game->life) {
        
        int x = 0;
        
        //  se l'utente ha troppe vite si mostrano in versione breve
        if (user->lives > 5) {
            
            output_image(game->life, PointOffset(point, x, 0));
            x += game->life->size.width;
            
            output_stringf(text_color, PointOffset(point, x, (game->life->size.height - h) / 2.), "x%d", user->lives);
            
        } else {
            
            int i;
            for (i = 0; i < user->lives; i++) {
                
                output_image(game->life, PointOffset(point, x, 0));
                x += game->life->size.width;
                
            }
            
        }
        
        point.y += game->life->size.height + spacing;
        
    }
}
