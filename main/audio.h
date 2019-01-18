#ifndef main_audio_h
#define main_audio_h

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "std/hashtable.h"

/**
 *  Tipi di file audio
 */
enum {

    /** Menu **/
    AUDIO_SAMPLE_INTRO,

    /** Un avversario è arrivato per primo alla fine del livello */
    AUDIO_SAMPLE_GAMEOVER,
    /** L'utente è arrivato per primo alla fine del livello */
    AUDIO_SAMPLE_GAMEWON,

    /** L'utente abbatte un muro */
    AUDIO_SAMPLE_WALL,
    /** Due personaggi si scontrano */
    AUDIO_SAMPLE_CRASH,

    AUDIO_SAMPLE_LAST

};

/**
 *  Formati di file audio supportati
 */
enum {

    AUDIO_SAMPLE_TYPE_NONE,

    AUDIO_SAMPLE_TYPE_MOD,
    AUDIO_SAMPLE_TYPE_WAV

};

/**
 *  File audio
 */
typedef struct audio_sample_s {

    /** Formato del file */
    int type;

    /** Livello di volume relativo */
    float gain;

    /** Se true, avvia un effetto di dissolvenza */
    bool fading;

    struct {
        
        /** MOD */
        ALLEGRO_AUDIO_STREAM * stream;

        /** WAV */
        ALLEGRO_SAMPLE * sample;
        ALLEGRO_SAMPLE_INSTANCE * instance;

    } track;

} audio_sample_t;

/** Array contenete la corrispondenza tra tipo di file audio e nome per la ricerca nella tabella */
extern char * audio_samples_names[];

/** Macro per la conversione di un tipo nel suo nome */
#define audio_sample_name(id)   (audio_samples_names[id])

/**
 *  Inizializza la gestione dell'audio caricando i suoni globali
 *
 *  @param config File di configurazione globale
 *
 *  @return Hashtable contenente i suoni
 */
hashtable_t * audio_initialize(hashtable_t * config);

/**
 *  Dealloca le strutture necessarie alla gestione dell'audio
 *
 *  @param table Hashtable dei suoni
 */
void audio_destroy(hashtable_t * table);

/**
 *  Carica una traccia audio e lo inserisce nella tabella
 *
 *  @param file Path del file
 *  @param table Tabella
 *  @param key Chiave per l'inserimento nella tabella
 */
void audio_sample_load(char * file, hashtable_t * table, char * key);

/**
 *  Suona una traccia audio
 *
 *  @param sample Riferimento alla traccia audio
 *  @param loop Se suonarla in loop
 */
void audio_sample_play(game_t * game, audio_sample_t * sample, bool loop);

/**
 *  Suona un file audio identificato dal suo id
 *
 *  @param game Contesto di gioco
 *  @param id Id della traccia audio
 *  @param loop Se suonarla in loop
 */
void audio_sample_play_by_id(game_t * game, int id, bool loop);

/**
 *  Ferma la riproduzione di una traccia audio
 *
 *  @param sample Riferimento alla traccia audio
 */
void audio_sample_stop(audio_sample_t * sample);

/**
 *  Ferma la riproduzione di una traccia audio identificata dal suo id
 *
 *  @param game Contesto di gioco
 *  @param id Id della traccia audio
 */
void audio_sample_stop_by_id(game_t * game, int id);

/**
 *  Avvia la dissolvenza di una traccia audio identificata dal suo id
 *
 *  @param game Contesto di gioco
 *  @param id Id della traccia audio
 */
void audio_sample_set_fading_by_id(game_t * game, int id);

/**
 *  Attiva/disattiva l'audio
 *
 *  @param game Contesto di gioco
 */
void audio_toggle_mute(game_t * game);

/**
 *  Esegue la dissolvenza delle tracce principali
 */
void audio_fade(game_t * game);

#endif
