#include "game/game.h"
#include "game/powerup.h"
#include "game/map.h"
#include "game/character.h"

#include "main/drawing.h"
#include "main/tiling.h"

#include "misc/random.h"

bool powerup_get(game_t * game, character_t * character, powerup_t * powerup)
{

    //  se il bonus non può essere raccolto dagli avversari e character è un avversario
    //  non si può proseguire oltre
    if (!character->is_user && !hashtable_search(powerup->picker, "enemy")) {
        return false;
    }

    //  stato del bonus, da associare al personaggio che lo ha raccolto
    powerup_status_t * status = NULL;

    //  si controlla se un bonus di questo tipo è già stato raccolto dall'utente
    foreach(character->powerups, powerup_status_t *, pwup) {
        //  i nomi coincidono
        if (!strcmp(powerup->name, pwup->powerup->name)) {
            status = pwup;
            break;
        }
    }

    //  se è la prima volta che il personaggio raccoglie il bonus si crea un nuovo stato associato
    if (!status) {
        status = powerup_status_new(powerup, character);
        //  si inserisce nella lista dei bonus dell'utente
        list_insert(character->powerups, status);
    } else
        //  si aggiorna il contatore della quantità di bonus posseduti
        status->counter++;

    //  se trigger è 0, il bonus si attiva automaticamente
    if (!powerup->trigger || !character->is_user) {
        powerup_enable(game, status);
    }

    return true;

}

void powerup_enable_setting(hashtable_t * config, void * data, void * key, void * value, struct type_functions value_type)
{

    character_t * character = data;

    //  aggiornamento delle proprietà dell'utente
    bool update = hashtable_replace(character->config, key, value);

    //  se non c'è una proprietà corrispondente si crea
    if (!update) {
        hashtable_insert(character->config, key, value, value_type);
    }

}

void powerup_disable_setting(hashtable_t * config, void * data, void * key, void * value, struct type_functions value_type)
{

    character_t * character = data;

    debugf("[Powerup] Reset proprietà %s\n", (char *)key);

    //  si cerca la proprietà corrispondente nei valori di default
    void * default_value = hashtable_search(character->default_config, key);

    //  se la proprietà non c'è
    if (!default_value)
        hashtable_insert(character->config, key, default_value, value_type);
    else
        //  aggiornamento delle proprietà dell'utente
        hashtable_replace(character->config, key, default_value);

}

void powerup_apply_properties(game_t * game, hashtable_t * config, character_t * character)
{

    if (!config)
        return;

    hashtable_iterate(config, character, powerup_enable_setting);

    //  quando il bonus viene utilizzato il personaggio è spostato in una posizione casuale della mappa
    if (bool_value(hashtable_search(config, "randomize_position")))
        character_set_random_position(character);

    //  bisogna mostrare il percorso più breve fino all'uscita?
    if (character->is_user && bool_value(hashtable_search(config, "show_shortest_path_to_exit"))) {

        ai_path_fiding_function find_path = ai_get_path_function("A*");
        find_path(game, character, character->map->end);

    }

    //  aumenta/diminuisce le vite del personaggio?
    if (character->is_user) {
        
        long * lives = hashtable_search(config, "lives");
        
        if (lives) {
            character->lives += *lives;
        }
        
    }
}

void powerup_reset_properties(game_t * game, hashtable_t * config, character_t * character)
{

    if (!config)
        return;

    hashtable_iterate(config, character, powerup_disable_setting);

}

void powerup_enable(game_t * game, powerup_status_t * status)
{

    character_t * character = status->character;

    //  se il bonus è già attivo non può essere riattivato
    if (status->enabled)
        return;

    //  se è "terminato" idem
    if (!status->counter)
        return;

    //  lista dei personaggi del livello
    list_t * characters = level_get_characters_list(game);

    //  attivazione del bonus
    status->enabled = 1;

    //  inizializzazione del tempo trascorso dall'attivazione
    status->elapsed = 0;

    //  applicazione degli effetti su chi raccoglie
    if (status->powerup->picker) {
        if (character->is_user) {

            //  configurazioni per l'utente che raccoglie il bonus
            powerup_apply_properties(game, hashtable_search(status->powerup->picker, "user"), character);

        } else {

            //  configurazioni per l'avversario che raccoglie il bonus
            powerup_apply_properties(game, hashtable_search(status->powerup->picker, "enemy"), character);

        }

    }

    //  applicazione degli effetti agli altri personaggi
    if (status->powerup->characters) {

        //  riquadro nel quale applicare gli effetti ai personaggi
        rectangle_t effects_rect;
        effects_rect.origin = PointZero;
        effects_rect.size = SizeMultiplyBySize(character->map->size, CellSize);

        if (!SizeIsZero(status->powerup->effects_rect_size)) {
            dimension_t rect_size = SizeMultiplyBySize(status->powerup->effects_rect_size, CellSize);
            effects_rect = rectangle_centered_make(character->position, rect_size, effects_rect.size);
        }

        //  per ogni personaggio
        foreach(characters, character_t *, chr) {
            //  diverso da chi ha raccolto il bonus
            if (chr == character)
                continue;

            //  il personaggio è fuori dall'area nella quale può subire effetti
            if (!RectContainsPoint(effects_rect, chr->position))
                continue;

            powerup_apply_properties(game, status->powerup->characters, chr);

        }
    }
    
    list_delete(characters);

    //  riproduzione di eventuali effetti sonori
    audio_sample_t * sample = hashtable_search(game->audio_samples, status->powerup->name);

    if (sample) {
        audio_sample_play(game, sample, false);
    }

}

void powerup_disable(game_t * game, powerup_status_t * status)
{

    //  personaggio che ha usato il bonus
    character_t * character = status->character;

    //  bonus
    powerup_t * powerup = status->powerup;
    
    //  lista dei personaggi del livello
    list_t * characters = level_get_characters_list(game);

    //  disabilitazione del bonus
    status->enabled = 0;

    //  ripristino delle proprietà su chi raccoglie
    if (powerup->picker) {

        if (character->is_user) {

            //  configurazioni per l'utente che raccoglie il bonus
            powerup_reset_properties(game, hashtable_search(powerup->picker, "user"), character);

        } else {

            //  configurazioni per l'avversario che raccoglie il bonus
            powerup_reset_properties(game, hashtable_search(powerup->picker, "enemy"), character);

        }
    }

    //  applicazione degli effetti agli altri personaggi
    if (powerup->characters) {
        foreach(characters, character_t *, chr) {

            //  diverso da chi ha raccolto il bonus
            if (chr == character) continue;

            powerup_reset_properties(game, powerup->characters, chr);
        }
    }
    
    list_delete(characters);

}

/**
 *  Posiziona un bonus in una cella della mappa tenendo conto di una certa probabilità
 *
 *  @param game Contesto di gioco
 *  @param powerup Bonus da posizionare
 *  @param map Mappa nella quale posizionare il bonus
 *	@param cell Cella nella quale posizionare il bonus
 */
bool powerup_place(game_t * game, powerup_t * powerup, map_t * map, map_cell_t * cell)
{

    //  ogni bonus ha una certa probabilità di comprarire
    if (!random_bool(powerup->appearance_probability / list_length(game->powerups)))
        return false;

    //  il bonus è stato utilizzato troppe volte
    if (powerup->placed >= powerup->limit)
        return false;

    //  aggiornamento dell'istante di inserimento dell'ultimo bonus
    map->powerup_time = time(NULL);

    //  collegamento cella <-> bonus
    cell->powerup.powerup = powerup;

    if (powerup->timeout > 0) {
        cell->powerup.place_time = time(NULL);
    }
    
    //  numero di volte che il bonus è comparso
    powerup->placed++;
    
    //
    return true;

}

void powerup_area(powerup_t * powerup, character_t * character)
{

    //  ha un raggio di azione?
    if (SizeIsZero(powerup->effects_rect_size))
        return;

    //  centro del riquadro di attivazione del bonus
    point_t center = PointOffset(character->position, CellSize.width / 2., CellSize.height / 2.);

    //  riquadro
    rectangle_t rect = rectangle_centered_make(
                                               center,
                                               SizeMultiplyBySize(powerup->effects_rect_size, CellSize),
                                               SizeMultiplyBySize(character->map->size, CellSize));

    //  colore
    color_t color = ColorArea;

    float alpha = 0.5;

    color.r *= alpha;
    color.g *= alpha;
    color.b *= alpha;
    color.a = 255 * alpha;

    rectangle_draw(rect, color);

}

/**
 *  Handler del timer che controlla lo stato dei bonus attivi e la comparsa di nuovi sulla mappa
 */
void powerups_handle_timer_tick(game_t * game, event_t event)
{

    if (!TimerEqualToTimer(game->powerups_timer, event.object))
        return;

    if (!game_is_started(game))
        return;

    //  lista dei personaggi del livello
    list_t * characters = level_get_characters_list(game);

    //  per ogni personaggio del livello
    foreach(characters, character_t *, character) {

        //  controllo dei bonus attualmente in possesso del personaggio
        foreach(character->powerups, powerup_status_t *, status) {

            //  vanno controllati solo quelli abilitati
            if (!status->enabled)
                continue;

            //  la funzione è eseguita circa ogni secondo, quindi si incrementa di 1 il tempo trascorso
            if (status->powerup->duration > 0)
                status->elapsed = (status->elapsed + 1) % status->powerup->duration;
            else
                status->elapsed = 0;

            //  tempo scaduto
            if (!status->elapsed) {
                //  lo stato del bonus va disabilitato
                powerup_disable(game, status);
                //  e decrementato il contatore
                status->counter--;
            }

        }

    }
    
    list_delete(characters);

    //  mappa nella quale si trova l'utente
    map_t * map = game->user->map;

    //  se è stato inserito un bonus troppo di recente non ne possono essere inseriti altri
    if (difftime(time(NULL), map->powerup_time) > map->powerups_time) {
        
        //  numero totale di bonus
        int powerups_count = 0;
        
        //  eventuale creazione di nuovi bonus
        foreach(map->powerup_cells, map_cell_t *, cell) {
            
            //  c'è già un bonus
            if (cell->powerup.powerup)
                continue;
            
            bool _break = false;

            foreach(game->powerups, powerup_t *, powerup) {
                if (powerup_place(game, powerup, map, cell)) _break = true;
            }
            
            //  c'è un bonus
            if (cell->powerup.powerup)
                powerups_count++;

            if (powerups_count >= map->powerups_limit || _break) break;
            
        }
        
    }

}

void powerup_check_cell(map_cell_t * cell) {
    
    powerup_t * powerup = cell->powerup.powerup;

    if (!(powerup->timeout > 0))
        return;
    
    double diff = difftime(time(NULL), cell->powerup.place_time);

    if (diff >= powerup->timeout) {
        cell->powerup.powerup = NULL;
    }
    
}

/**
 *  Handler dell'evento EVENT_POWERUP_USE
 */
void powerup_handle_use(game_t * game, event_t event)
{

    powerup_status_t * status = event.object;

    if (!status)
        return;

    powerup_enable(game, status);

}

powerup_t * powerup_new(game_t * game, char * name, hashtable_t * config)
{

    //  configurazione delle properietà
    hashtable_t * characters = hashtable_search(config, "characters");
    hashtable_t * picker = hashtable_search(config, "picker");

    if (!picker && !characters)
        return NULL;

    //  file contenente l'immagine che rappresenta il bonus
    char * texture_file = hashtable_search(config, "texture_file");

    if (!texture_file)
        return NULL;

    //  posizione del'immagine del bonus nella bitmap
    rectangle_t * texture_rect = hashtable_search(config, "texture_rect");

    if (!texture_rect)
        return NULL;

    //  caricamento dell'immagine che rappresenta il bouns sulla mappa
    image_t * texture = image_load_new(texture_file, *texture_rect, true);

    if (!texture)
        return NULL;

    //
    powerup_t * powerup = memalloc(powerup_t, 1, true);
    
    //  copia del nome
    powerup->name = strdup(name);
    
    //  file audio
    char * audio = hashtable_search(config, "audio");
    
    if (audio)
        audio_sample_load(audio, game->audio_samples, powerup->name);
    
    //  tile
    powerup->tile = texture;

    //  probabilità di apparizione nelle celle destinate ai bonus
    powerup->appearance_probability = float_value(hashtable_search(config, "appearance_probability"));

    //  durata del bonus in secondi
    long * duration = hashtable_search(config, "duration");
    powerup->duration = long_value(duration);
    
    //  tempo prima della scomparsa
    long * timeout = hashtable_search(config, "timeout");
    powerup->timeout = long_value(timeout);

    //  numero massimo di bonus (di questo tipo) per livello
    long * limit = hashtable_search(config, "limit");
    powerup->limit = (limit ? *limit : LONG_MAX);

    //  proprietà
    powerup->picker = picker ? hashtable_duplicate(picker) : NULL;
    powerup->characters = characters ? hashtable_duplicate(characters) : NULL;

    //  dimensione del riquadro all'interno del quale far registrare gli effetti sui personaggi
    powerup->effects_rect_size = dimension_value(hashtable_search(config, "effects_rect_size"));

    //  tasto che attiva il bonus
    char * trigger = hashtable_search(config, "trigger");
    powerup->trigger = (trigger && *trigger) ? *trigger : 0;

    return powerup;

}

void powerup_delete(powerup_t * powerup)
{

    //  1. deallocazione del nome
    memfree(powerup->name);

    //  2. deallocazione della bitmap
    image_delete(powerup->tile);

    //  3. proprietà di chi raccoglie
    hashtable_delete(powerup->picker);

    //  4. proprietà degli altri
    hashtable_delete(powerup->characters);

    //  5. deallocazione del bonus
    memfree(powerup);

}

list_t * powerups_load(game_t * game, hashtable_t * config)
{

    //  lista dei nomi delle variabili contenenti le informazioni sui bonus da caricare
    list_t * powerups_names = hashtable_search(config, "powerups");

    if (!powerups_names)
        return NULL;

    //  creazione della lista dei bonus (powerup_t)
    list_t * powerups = list_new(powerup_functions);

    //  per ogni nome nella lista dei nomi
    foreach(powerups_names, char *, powerup_name) {

        //  si cerca la variabile contenente le informazioni nel file di configurazione
        hashtable_t * powerup_config = hashtable_search(config, powerup_name);

        //  se non esiste questa variabile si passa al prossimo bonus
        if (!powerup_config)
            continue;

        //  si crea quel bonus
        powerup_t * powerup = powerup_new(game, powerup_name, powerup_config);

        //  si inserisce il bonus nella lista dei bonus creati
        if (powerup) {
            debugf("[Bonus] %s caricato\n", powerup_name);
            list_insert(powerups, powerup);
        }

    }

    return powerups;

}

powerup_status_t * powerup_status_new(powerup_t * powerup, character_t * character)
{

    powerup_status_t * status = memalloc(powerup_status_t);

    //  collegamento bonus - stato
    status->powerup = powerup;

    //  collegamento personaggio - stato
    status->character = character;

    //  valori iniziali
    status->counter = 1;
    status->elapsed = 0;
    status->enabled = 0;

    return status;

}

void powerup_status_delete(powerup_status_t * status)
{

    //  semplice dellocazione
    memfree(status);

}

void powerups_register_event_handlers(game_t * game)
{
    
    //  gestore dei timer
    event_subscribe(game, EVENT_TIMER_TICK, powerups_handle_timer_tick);

    //  bonus utilizzato
    event_subscribe(game, EVENT_POWERUP_USE, powerup_handle_use);

}

TYPE_FUNCTIONS_DEFINE(powerup, powerup_delete);
TYPE_FUNCTIONS_DEFINE(powerup_status, powerup_status_delete);
