#ifndef game_powerup_h
#define game_powerup_h

#include "types.h"

#include "std/list.h"
#include "std/hashtable.h"

#include "main/graphics.h"
#include "main/audio.h"

#include "game/character.h"

/**
 *  Contiene informazioni sugli effetti che un bonus causa ai personaggi
 */
struct powerup_s {

    /** Bitmap che rappresenta il bonus */
    image_t * tile;

    /** Nome, univoco, del bonus */
    char * name;

    /** Probabilità di apparizione del bonus (0.0 ... 1.0) */
    float appearance_probability;

    /** Dimensione del riquadro all'interno del quale far registrare gli effetti sui personaggi */
    dimension_t effects_rect_size;

    /** Durata, in secondi, del bonus */
    long duration;
    
    /** Secondi prima della scomparsa del bonus */
    long timeout;
    
    /** Tasto che attiva il bonus (A-Z). Se non definito il bonus è attivato immediatamente */
    int trigger;

    /** Numero massimo di bonus di questo tipo per livello */
    long limit;

    /** Numero di volte che questo bonus è stato posizionato sulla mappa */
    long placed;

    /** Proprietà da applicare ai personaggi che raccolgono il bonus */
    hashtable_t * picker;

    /** Proprietà da applicare ai personaggi che NON raccolgono il bonus */
    hashtable_t * characters;

};

/**
 *  Contiene informazioni sui bonus raccolti da un personaggio
 */
struct powerup_status_s {

    /** Bonus raccolto */
    powerup_t * powerup;

    /** Stato on/off */
    bool enabled;

    /** Numero di volte che il bonus è stato raccolto dall'utente */
    int counter;

    /** Tempo trascorso, in secondi, dall'ultimo utilizzo */
    long elapsed;

    /** Personaggio al quale è associato lo stato */
    character_t * character;

};

/**
 *  Contiene informazioni sul bonus posizionato in una cella
 */
struct powerup_location_s {
    
    /** Bonus posizionato */
    powerup_t * powerup;

    /** Istante in cui il bonus è stato posizonato */
    time_t place_time;
};

/**
 *  Creazione di un nuovo bonus (powerup_t) utilizzando le informazioni contenute
 *  nella hashtable di configurazione _config_ per il bonus di nome _name_
 *
 *  @param game Contesto di gioco
 *  @param name Nome della variabile di tipo dictionary che nel file di configurazione contiene le informazioni riguardanti il bonus
 *  @param config Hashtable contenente la configurazione dei bonus
 *
 *  @return Bonus
 */
powerup_t * powerup_new(game_t * game, char * name, hashtable_t * config);

/**
 *  Deallocazione di un bonus
 *
 *  @param powerup Bonus da deallocare
 */
void powerup_delete(powerup_t * powerup);

/**
 *  Caricamento dei bonus dal file di configurazione principale
 *
 *  @param game Contesto di gioco
 *  @param config Hashtable contenente la configurazione dei bonus
 *
 *  @return Lista dei bonus (powerup_t *) caricati
 *
 *  @retval NULL In caso di errori
 */
list_t * powerups_load(game_t * game, hashtable_t * config);

/**
 *  Caricamento degli effetti sonori associati ai bonus
 *
 *  @param game Contesto di gioco
 */
void powerups_load_audio_samples(game_t * game);

/**
 *  Assegnazione di un bonus ad un personaggio
 *
 *  @param game Contesto di gioco
 *  @param character Personaggio al quale assegnare il bonus
 *  @param powerup Bonus da assegnare
 *
 *  @return risultato dell'assegnazione del bonus
 *  @retval false bonus non assegnato
 *  @retval true bonus assegnato
 */
bool powerup_get(game_t * game, character_t * character, powerup_t * powerup);

/**
 *  Attivazione di un bonus (alla pressione di un tasto o quando viene raccolto) per un personaggio
 *
 *  @param game Contesto di gioco
 *  @param powerup Bonus da attivare
 */
void powerup_enable(game_t * game, powerup_status_t * powerup);

/**
 *  Evidenzia l'area di validità di un bonus
 *
 *  @param powerup Bonus
 *  @param character Personaggio che utilizza il bonus
 */
void powerup_area(powerup_t * powerup, character_t * character);

/**
 *  Creazione di un nuovo stato per un bonus (powerup_status_t)
 *
 *  @param powerup Bonus al quale associare lo stato
 *  @param character Personaggio al quale associare lo stato
 *
 *  @return Stato
 */
powerup_status_t * powerup_status_new(powerup_t * powerup, character_t * character);

/**
 *  Controlla se il bonus in una certa cella è ancora valido e in caso
 *  non lo sia lo rimuove
 *
 *  @param cell Cella da controllare
 */
void powerup_check_cell(map_cell_t * cell);

/**
 *  Registra gli handler degli eventi
 *
 *  @param game Contesto di gioco
 */
void powerups_register_event_handlers(game_t * game);

#endif
