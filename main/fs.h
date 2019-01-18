#ifndef main_fs_h
#define main_fs_h

#include <allegro5/allegro.h>

/**
 *  Distruzione del percorso della cartella delle risorse
 */
void fs_destroy(void);

/**
 *  Separatore di percorsi per la piattaforma corrente
 */
#define FS_PATH_SEPARATOR ALLEGRO_NATIVE_PATH_SEP

/**
 *  Percorso della cartella delle risorse (files grafici, audio, livelli, mappe, bonus, configurazione)
 *
 *  @return Path
 */
const char * fs_get_resources_path(void);

/**
 *  Dati un certo numero di stringhe le concatena a costruire un percorso
 *
 *  @param first Prima componente
 *  @param ... Altre componenti
 *
 *  @return Stringa costituita dalle componenti concatenate alternate da un separatore
 */
#define fs_construct_path(first, ...) _fs_construct_path(first, __VA_ARGS__, NULL)

char * _fs_construct_path(const char * first, ...);

#endif
