#ifndef main_output_h
#define main_output_h

#include "utils.h"

#include "game/events.h"
#include "game/game.h"

#include "main/image.h"

#include "misc/geometry.h"

/**
 *  Effettua il refresh dello buffer dello schermo
 */
void output_screen_redraw(void);

/**
 *  Gestisce le richieste di aggiornamento dello schermo
 */
void output_handle_screen_redraw(game_t * game, event_t event);

/**
 *  Riempie lo schermo con un colore
 *
 *  @param color Colore
 */
void output_screen_fill(color_t color);

/**
 *  Disegna un'immagine ad una certa locazione
 *
 *  @param image Immagine da eliminare
 *  @param location Posizione alla quale disegnare l'immagine (in pixel)
 */
void output_image(image_t * image, point_t location);

/**
 *  Disegna un'immagine ad una certa locazione con una data opacità
 *
 *  @param image Immagine da eliminare
 *  @param location Posizione alla quale disegnare l'immagine (in pixel)
 *  @param opacity Opacità (0-255)
 */
void output_image_opacity(image_t * image, point_t location, unsigned char opacity);

/**
 *  Disegna una parte di un'immagine ad una certa locazione
 *
 *  @param image Immagine da eliminare
 *  @param source Rettangolo che delimita l'area da disegnare
 *  @param location Posizione alla quale disegnare l'immagine (in pixel)
 */
void output_image_tile(image_t * image, rectangle_t source, point_t location);

/**
 *  Disegna una parte di un'immagine ad una certa locazione con una data tinta
 *
 *  @param image Immagine da eliminare
 *  @param source Rettangolo che delimita l'area da disegnare
 *  @param location Posizione alla quale disegnare l'immagine (in pixel)
 *  @param color Tinta
 */
void output_image_tile_tinted(image_t * image, rectangle_t source, point_t location, color_t color);

/**
 *  Effettua la traslazione dell'origine degli assi in modo tale che la mappa risulti centrata
 *
 *  @param origin Nuova origine
 */
void output_set_origin(point_t origin);

/**
 *  Effettua lo scaling del contenuto dello schermo
 *
 *  @param scale Nuova proporzione
 */
void output_set_scale(dimension_t scale);

/**
 *  Scrive una stringa sullo schermo in modo simile a printf
 *
 *  @param color Colore
 *  @param location Posizione alla quale disegnare la stringa
 *  @param fmt Formato
 *  @param ... Parametri da formattare
 */
void output_stringf(color_t color, point_t location, char * fmt, ...);

/**
 *  Colora lo schermo di nero e scrive una stringa al centro
 *
 *  @param title Titolo
 *  @param messaggio Messaggio
 *  @param color Colore della stringa
 */
void output_fullscreen_message(char * title, char * message, color_t color);

/**
 *  Ritorna le dimensioni necessarie per la scrittura di una stringa
 *
 *  @param string Stringa da scrivere
 *
 *  @return Dimensioni
 */
dimension_t output_get_string_size(char * string);

/**
 *  Ridisegna la mappa:
 *  - sfondo
 *  - mura
 *  - bonus
 *
 *  @param level Livello di appartenenza della mappa
 *  @param map Mappa
 */
void output_map(level_t * level, map_t * map);

#endif
