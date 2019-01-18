#include "game/level.h"
#include "game/map.h"
#include "game/game.h"

#include "config/config.h"

#include "main/audio.h"
#include "main/drawing.h"
#include "main/fs.h"
#include "main/graphics.h"

#include "misc/random.h"

list_t * level_get_characters_list(game_t * game)
{

    list_t * characters = list_new(no_functions);

    //  inserimento dell'utente
    list_insert(characters, game_get_user(game), INSERT_MODE_TAIL, false, false);

    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    //  inserimento degli avversari
    foreach(level->enemies, character_t *, character) {
        list_insert(characters, character, INSERT_MODE_TAIL, false, false);
    }

    return characters;

}

/**
 *  Handler per l'evento EVENT_MAP_NEXT, eseguito al cambio di mappa.
 *  Il gioco è messo in pausa ed è avviata l'animazione di fading out.
 */
void level_handle_next_map(game_t * game, event_t evt)
{

    game_set_paused(game);
    
    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    animation_start(game, level->animations[LEVEL_ANIMATION_MAP_TRANSITION_OUT]);

}

/**
 *  Handler per l'evento EVENT_ANIMATION_ENDED, eseguito al termine di un'animazione.
 *  Quando termina il fading-out si cambia mappa e si esegue il fading-in.
 */
void level_handle_animation_event(game_t * game, event_t evt)
{

    //  livello corrente
    level_t * level = game_get_current_level(game);
    
    if (!level)
        return;
    
    //  è terminata l'animazione di fadeing-out
    if (evt.object == level->animations[LEVEL_ANIMATION_MAP_TRANSITION_OUT]) {

        //  cambio mappa e avvio fading-in
        character_t * user = game_get_user(game);
        
        character_clear_path(user);
        
        //character_goto_next_map(game, user);
        character_set_map(user, user->map->next);
        character_decide_direction_user(user);
        
        animation_start(game, level->animations[LEVEL_ANIMATION_MAP_TRANSITION_IN]);

    } else if (evt.object == level->animations[LEVEL_ANIMATION_MAP_TRANSITION_IN]) {

        //  terminato il fading-in sul nuovo livello si riavvia il gioco
        game_set_running(game);

    }

}

void level_setup(game_t * game, level_t * level)
{

    //  inizializzazioni animazioni
    level->animations[LEVEL_ANIMATION_MAP_TRANSITION_OUT] = animation_new(1, 30, NULL, animation_animate_fading_out);
    level->animations[LEVEL_ANIMATION_MAP_TRANSITION_IN] = animation_new(1, 30, NULL, animation_animate_fading_in);

    animation_enlist(game, level->animations[LEVEL_ANIMATION_MAP_TRANSITION_OUT]);
    animation_enlist(game, level->animations[LEVEL_ANIMATION_MAP_TRANSITION_IN]);

    //  prima mappa
    map_t * first = list_head(level->maps);

    if (!first)
        return;

    //  utente
    character_t * user = game_get_user(game);
    
    //  posizionamento del personaggio dell'utente
    character_set_map(user, first);

    //  posizionamento avversari
    foreach(level->enemies, character_t *, enemy) {
        
        //  assegnazione mappa
        character_set_map(enemy, first);
        
    }
    
    //  posizionamento
    map_place_enemies(level, first);
    
    //  avvio musica
    audio_sample_t * audio = hashtable_search(game->audio_samples, level->name);

    if (audio)
        audio_sample_play(game, audio, true);

}

void level_uninstall(game_t * game, level_t * level)
{

    //  animazioni
    int i;
    for (i = 0; i < LEVEL_ANIMATION_LAST; i++) {
        animation_delist(game, level->animations[i]);
    }

    //  stop musica
    audio_sample_t * audio = hashtable_search(game->audio_samples, level->name);

    if (audio)
        audio_sample_stop(audio);

}

/**
 *  Creazione di un nuovo livello caricando il file di configurazione _config_file_path_
 *  presente nella cartella _level_dir_.
 *
 *  @param game Contesto di gioco
 *  @param level_dir Cartella contenente il file di configurazione del livello
 *  @param config_file_path File di configurazione del livello
 *
 *  @return Livello
 *
 *  @retval NULL In caso di errore
 */
level_t * level_new(game_t * game, char * level_dir, char * config_file_path)
{

    hashtable_t * config = config_open(config_file_path);

    //  se non c'è il file di conf è come se il livello non esistesse
    if (!config)
        return NULL;

    //  caricamento mappe
    list_t * maps_list = hashtable_search(config, "maps");

    //  niente mappe
    if (!maps_list) {

        hashtable_delete(config);
        return NULL;

    }

    level_t * level = memalloc(level_t, 1, true);

    //  nome del livello
    char * name = hashtable_search(config, "name");
    level->name = name ? strdup(name) : random_string(8);

    //  audio file
    char * audio = hashtable_search(config, "audio");

    if (audio)
        audio_sample_load(audio, game->audio_samples, level->name);
    
    //  caricamento delle texture per l'ambiente (terreno, mura)
    //  path della bitmap delle mura
    char * wall_bmp_path = hashtable_search(config, "wall_texture");

    //  niente mura
    if (!wall_bmp_path) {

        level_delete(level);
        hashtable_delete(config);
        return NULL;
        
    }

    //  posizione delle mura nella bitmap
    rectangle_t * rect = hashtable_search(config, "wall");

    //  caricamento bitmap delle mura
    level->textures[LEVEL_TEXTURE_WALL] = image_load_new(wall_bmp_path, *rect, true);
   
    //  impossibile caricare
    if (!level->textures[LEVEL_TEXTURE_WALL]) {
        
        level_delete(level);
        hashtable_delete(config);
        return NULL;
        
    }
    
    //  path della bitmap dello sfondo
    char * background_bmp_path = hashtable_search(config, "background_texture");

    //  niente bmp dei percorsi
    if (!background_bmp_path) {

        level_delete(level);
        hashtable_delete(config);
        return NULL;
        
    }

    //  posizione dello sfondo nella bitmap
    rect = hashtable_search(config, "background");

    //  immagine
    image_t * background_image = image_load_new(background_bmp_path, *rect, true);

    //  le immagini per i percorsi sono necessarie
    if (!background_image) {
        
        level_delete(level);
        hashtable_delete(config);
        return NULL;
        
    }

    //  texture per una cella di tipo sconosciuto, colore nero
    level->textures[CELL_TYPE_UNKNOWN] = image_create_new(ColorMakeRGB(0, 0, 0), CellSize);

    //  crea una versione delle celle per ogni valore nel range [1-9] dei valori delle celle
    //  per motivi prestazionali è più conveniente creare copie a priori che ri-tinteggiare tutto quando si disegna la mappa
    int i = 0;
    for (i = 0; i <= LEVEL_TEXTURE_PATH_DARKEST - LEVEL_TEXTURE_PATH_LIGHTEST; i++) {
        if (i == CellDefaultValue - 1)
            level->textures[LEVEL_TEXTURE_PATH_LIGHTEST + i] = background_image;
        else
            level->textures[LEVEL_TEXTURE_PATH_LIGHTEST + i] = image_duplicate_tinted(background_image, cell_tint(i + 1));
    }

    //  caricamento degli avversari
    level->enemies = characters_load(config, "enemies");

    //  complessità
    float complexity = float_value(hashtable_search(config, "complexity"));
    level->complexity = RangeContainsValue(RangeMake(0., 1.), complexity) ? complexity : 0.1;

    //  costruzione lista delle mappe
    level->maps = list_new(map_functions);

    if (!level->maps) {
        hashtable_delete(config);
        level_delete(level);
        return NULL;
    }

    //  posizione delle celle di entrata/uscita (asse x/y)
    int special_cells = random_int(0, 1);

    //  ultima mappa
    map_t * last_map = NULL;

    //  caricamento mappe
    foreach(maps_list, char *, map_config_file) {

        map_t * map = NULL;

        //  generazione di una mappa casuale
        if (!strncasecmp(map_config_file, "RANDOM", 6)) {

            dimension_t screen = graphics_get_screen_size();

            //  dimensioni massime per altezza e larghezza
            //  (la mappa generata avrà dimensione effettiva pari a circa il doppio)
            int max_width = screen.width / (CellSize.width * 2) - 3;
            int max_height = screen.height / (CellSize.height * 2) - 3;

            dimension_t map_size = SizeMake(random_int(9, max_width), random_int(9, max_height));

            map = map_generate(level, map_size, special_cells, random_bool(0.5), map_config_file);
            map->next = NULL;

        } else {
            
            //  percorso del file di configurazione
            char * map_config_file_path = fs_construct_path(level_dir, map_config_file);

            //  caricamento mappa
            map = map_load_new(level, map_config_file_path);

            memfree(map_config_file_path);

        }

        //  mappa caricata
        if (map) {
            list_insert(level->maps, map, INSERT_MODE_TAIL, false, true);

            if (last_map)
                last_map->next = map;

            last_map = map;
        }

    }

    //  deallocazione configurazione
    hashtable_delete(config);
    
    //  se non è stata caricata alcuna mappa ritorna null
    if (!list_length(level->maps)) {
        level_delete(level);
        level = NULL;
    }

    debugf("[Livello] %s caricato (%zu mappe)\n", level->name, list_length(level->maps));

    return level;

}

queue_t * levels_load(game_t * game, hashtable_t * config)
{

    //  percorso dei file di configurazione dei livelli
    char * levels_dir  = hashtable_search(config, "levels_path");

    if (!levels_dir)
        return NULL;

    //  nomi dei livelli
    list_t * levels_names = hashtable_search(config, "levels");

    if (!levels_names)
        return NULL;

    //  lista delle configurazioni (level_t)
    queue_t * levels = queue_new();

    if (!levels)
        return NULL;

    //  per ogni livello
    foreach(levels_names, char *, level_name) {

        //  costruzione path del file di configurazione
        char * level_dir_path   = fs_construct_path(fs_get_resources_path(), levels_dir, level_name);
        char * config_file_path = fs_construct_path(fs_get_resources_path(), levels_dir, level_name, level_name, ".cfg");

        //  costruzione livello
        level_t * level = level_new(game, level_dir_path, config_file_path);

        //  se il caricamento non ha avuto problemi si inserisce nella lista
        if (level) {
            queue_push(levels, level);
        }

        memfree(config_file_path);
        memfree(level_dir_path);

    }

    //  se non è stata caricata alcun livello ritorna null
    if (queue_empty(levels)) {
        queue_delete(levels);
        levels = NULL;
    }

    return levels;

}

//  deallocazione di un livello
void level_delete(level_t * level)
{

    if (!level)
        return;

    //  1. deallocazione mappe
    list_delete(level->maps);

    //  2. textures
    int i;
    for (i = 0; i < LEVEL_TEXTURE_LAST; i++) {
        image_delete(level->textures[i]);
    }

    //  3. avversari
    list_delete(level->enemies);

    //  4. nome
    memfree(level->name);

    //  5. livello
    memfree(level);

}

void levels_register_event_handlers(game_t * game)
{

    //  evento "cambiamento mappa"
    event_subscribe(game, EVENT_MAP_NEXT, level_handle_next_map);

    //  handler degli eventi sulle animazioni
    event_subscribe(game, EVENT_ANIMATION_ENDED, level_handle_animation_event);

}

TYPE_FUNCTIONS_DEFINE(level, level_delete);

