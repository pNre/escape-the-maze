
#include <stdlib.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "game/game.h"

#include "main/audio.h"
#include "main/fs.h"

#include "utils.h"

char * audio_samples_names[] = {
    [AUDIO_SAMPLE_INTRO]    = "Intro",

    [AUDIO_SAMPLE_GAMEOVER] = "Game Over",
    [AUDIO_SAMPLE_GAMEWON]  = "Game Won",

    [AUDIO_SAMPLE_WALL]     = "Wall",
    [AUDIO_SAMPLE_CRASH]    = "Crash"
};

ALLEGRO_MIXER * mixer = NULL;
ALLEGRO_VOICE * voice = NULL;

void audio_sample_delete(audio_sample_t * sample)
{

    if (sample->type != AUDIO_SAMPLE_TYPE_NONE) {

        if (sample->type == AUDIO_SAMPLE_TYPE_WAV) {
            al_destroy_sample_instance(sample->track.instance);
            al_destroy_sample(sample->track.sample);
        } else {
            al_destroy_audio_stream(sample->track.stream);
        }

    }

    memfree(sample);

}

TYPE_FUNCTIONS_DEFINE(audio_samples, audio_sample_delete);

void audio_sample_play_by_id(game_t * game, int id, bool loop)
{

    audio_sample_t * sample = hashtable_search(game->audio_samples, audio_sample_name(id));

    if (!sample)
        return;

    audio_sample_play(game, sample, loop);

}

void audio_sample_stop_by_id(game_t * game, int id)
{

    audio_sample_t * sample = hashtable_search(game->audio_samples, audio_sample_name(id));

    if (!sample)
        return;

    audio_sample_stop(sample);

}

void audio_sample_play(game_t * game, audio_sample_t * sample, bool loop)
{
    
    if (!al_is_audio_installed())
        return;

    float gain = game->mute ? 0. :  1.;
    
    if (sample->type == AUDIO_SAMPLE_TYPE_MOD) {
        al_set_audio_stream_gain(sample->track.stream, gain);
        al_attach_audio_stream_to_mixer(sample->track.stream, mixer);
        al_set_audio_stream_playmode(sample->track.stream, (loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE));
    }
    else {
        al_set_sample_instance_gain(sample->track.instance, gain);
        al_play_sample_instance(sample->track.instance);
        al_set_sample_instance_playmode(sample->track.instance, (loop ? ALLEGRO_PLAYMODE_LOOP : ALLEGRO_PLAYMODE_ONCE));
    }

}

void audio_sample_stop(audio_sample_t * sample)
{

    if (sample->type == AUDIO_SAMPLE_TYPE_MOD)
        al_detach_audio_stream(sample->track.stream);
    else if (sample->type == AUDIO_SAMPLE_TYPE_WAV)
        al_stop_sample_instance(sample->track.instance);

}

void audio_sample_set_fading_by_id(game_t * game, int id)
{

    audio_sample_t * sample = hashtable_search(game->audio_samples, audio_sample_name(id));

    if (!sample)
        return;

    sample->fading = true;

}

hashtable_t * audio_initialize(hashtable_t * config)
{

    //  inizializzazione della libreria audio
    if (!al_install_audio())
        return NULL;

    if (!al_init_acodec_addon()) {
        al_uninstall_audio();
        return NULL;
    }

    voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    
    if (!voice)
        return NULL;
    
    mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    
    if (!mixer)
        return NULL;
    
    al_attach_mixer_to_voice(mixer, voice);
    
    //  tabella dei files audio
    hashtable_t * samples_table = hashtable_new(HASHTABLE_DEFAULT_SIZE, string_functions);

    //  caricamento effetti sonori del gioco
    hashtable_t * sfx = hashtable_search(config, "sfx");

    if (sfx) {

        //  effetto "game over", in caso di sconfitta
        char * gameover = hashtable_search(sfx, "gameover");

        if (gameover)
            audio_sample_load(gameover, samples_table, audio_sample_name(AUDIO_SAMPLE_GAMEOVER));

        //  effetto "vittoria"
        char * won = hashtable_search(sfx, "won");

        if (won)
            audio_sample_load(won, samples_table, audio_sample_name(AUDIO_SAMPLE_GAMEWON));

        //  muro abbattuto
        char * wall = hashtable_search(sfx, "wall");

        if (wall)
            audio_sample_load(wall, samples_table, audio_sample_name(AUDIO_SAMPLE_WALL));

        //  scontro tra personaggi
        char * crash = hashtable_search(sfx, "crash");

        if (crash)
            audio_sample_load(crash, samples_table, audio_sample_name(AUDIO_SAMPLE_CRASH));

        //  schermata principale
        char * intro = hashtable_search(sfx, "intro");

        if (intro)
            audio_sample_load(intro, samples_table, audio_sample_name(AUDIO_SAMPLE_INTRO));

    }

    return samples_table;

}

void audio_destroy(hashtable_t * table)
{

    //  si prosegue solo se è stato chiamato prima audio_initialize
    //  e ha inizializzato tutto con successo
    if (!al_is_audio_installed())
        return;
    
    if (mixer)
        al_destroy_mixer(mixer);
    
    if (voice)
        al_destroy_voice(voice);

    hashtable_delete(table);

    al_uninstall_audio();

}

void audio_sample_load(char * file, hashtable_t * table, char * key)
{
    
    if (!al_is_audio_installed())
        return;

    audio_sample_t * sample = memalloc(audio_sample_t);

    sample->type = AUDIO_SAMPLE_TYPE_NONE;
    sample->gain = 1.;

    //  si controlla l'estensione
    char * extension = file;
    extension += strlen(file) - 3;

    char * absolute_path = fs_construct_path(fs_get_resources_path(), file);

    //  se è un wav
    if (!strcasecmp(extension, "wav")) {

        //  si carica la traccia
        ALLEGRO_SAMPLE * al_sample = al_load_sample(absolute_path);

        if (!al_sample) {
            memfree(absolute_path);
            audio_sample_delete(sample);
            return;
        }

        sample->track.sample = al_sample;
        sample->track.instance = al_create_sample_instance(al_sample);
        
        sample->type = AUDIO_SAMPLE_TYPE_WAV;

        al_attach_sample_instance_to_mixer(sample->track.instance, mixer);
        
    } else {    //  altrimenti sarà un mod/xm

        //  si apre un flusso
        ALLEGRO_AUDIO_STREAM * al_stream = al_load_audio_stream(absolute_path, 4, 1024 * 2);

        if (!al_stream) {
            memfree(absolute_path);
            audio_sample_delete(sample);
            return;
        }

        sample->track.stream = al_stream;
        sample->type = AUDIO_SAMPLE_TYPE_MOD;

    }

    //  se il file è caricato con successo si inserisce nella tabella
    hashtable_insert(table, key, sample, audio_samples_functions, 1);

    debugf("[Audio] %s (%s) caricato\n", file, key);
    
    memfree(absolute_path);

}

void audio_mute_sample(hashtable_t * samples, void * data, void * key, void * value, struct type_functions type)
{
    game_t * game = (game_t *)data;
    
    audio_sample_t * sample = (audio_sample_t *)value;
    
    if (sample->type == AUDIO_SAMPLE_TYPE_MOD) {
        al_set_audio_stream_gain(sample->track.stream, (game->mute ? 0. : 1.));
    }
    
}

void audio_toggle_mute(game_t * game)
{
    
    game->mute = !game->mute;
    
    if (!game->audio_samples)
        return;
    
    //  si controlla ogni elemento nell'hashtable delle tracce caricate
    hashtable_iterate(game->audio_samples, game, audio_mute_sample);
    
}

void audio_fade_sample(hashtable_t * samples, void * data, void * key, void * value, struct type_functions type)
{

    audio_sample_t * sample = (audio_sample_t *)value;

    //  il fading è possibile solo sui mod
    if (sample->type != AUDIO_SAMPLE_TYPE_MOD)
        return;
    
    //  se l'elemento non è in fading si passa al prossimo
    if (!sample->fading)
        return;

    //  si riduce progressivamente il livello dell'audio da 1. a 0.
    al_set_audio_stream_gain(sample->track.stream, sample->gain);
    sample->gain -= 0.01;

    //  superato lo 0, si ferma la traccia e si resetta il volume
    if (sample->gain < 0) {
        audio_sample_stop(sample);
        sample->fading = false;
        sample->gain = 1.;
    }

}

void audio_fade(game_t * game)
{

    if (!game->audio_samples)
        return;
    
    if (game->mute)
        return;
    
    //  si controlla ogni elemento nell'hashtable delle tracce caricate
    hashtable_iterate(game->audio_samples, NULL, audio_fade_sample);

}
