#ifndef io_keyboard_h
#define io_keyboard_h

#include <allegro5/allegro5.h>

#define KYB_KEY_UP      ALLEGRO_KEY_UP
#define KYB_KEY_DOWN    ALLEGRO_KEY_DOWN
#define KYB_KEY_LEFT    ALLEGRO_KEY_LEFT
#define KYB_KEY_RIGHT   ALLEGRO_KEY_RIGHT

#define KYB_KEY_ESCAPE  ALLEGRO_KEY_ESCAPE
#define KYB_KEY_ENTER   ALLEGRO_KEY_ENTER
#define KYB_KEY_SPACE   ALLEGRO_KEY_SPACE

#define KYB_KEY_0       ALLEGRO_KEY_0
#define KYB_KEY_1       ALLEGRO_KEY_1
#define KYB_KEY_2       ALLEGRO_KEY_2
#define KYB_KEY_3       ALLEGRO_KEY_3
#define KYB_KEY_4       ALLEGRO_KEY_4
#define KYB_KEY_5       ALLEGRO_KEY_5
#define KYB_KEY_6       ALLEGRO_KEY_6
#define KYB_KEY_7       ALLEGRO_KEY_7
#define KYB_KEY_8       ALLEGRO_KEY_8
#define KYB_KEY_9       ALLEGRO_KEY_9

#define KYB_KEY_A       ALLEGRO_KEY_A
#define KYB_KEY_B       ALLEGRO_KEY_B
#define KYB_KEY_C       ALLEGRO_KEY_C
#define KYB_KEY_D       ALLEGRO_KEY_D
#define KYB_KEY_E       ALLEGRO_KEY_E
#define KYB_KEY_F       ALLEGRO_KEY_F
#define KYB_KEY_G       ALLEGRO_KEY_G
#define KYB_KEY_H       ALLEGRO_KEY_H
#define KYB_KEY_I       ALLEGRO_KEY_I
#define KYB_KEY_J       ALLEGRO_KEY_J
#define KYB_KEY_K       ALLEGRO_KEY_K
#define KYB_KEY_L       ALLEGRO_KEY_L
#define KYB_KEY_M       ALLEGRO_KEY_M
#define KYB_KEY_N       ALLEGRO_KEY_N
#define KYB_KEY_O       ALLEGRO_KEY_O
#define KYB_KEY_P       ALLEGRO_KEY_P
#define KYB_KEY_Q       ALLEGRO_KEY_Q
#define KYB_KEY_R       ALLEGRO_KEY_R
#define KYB_KEY_S       ALLEGRO_KEY_S
#define KYB_KEY_T       ALLEGRO_KEY_T
#define KYB_KEY_U       ALLEGRO_KEY_U
#define KYB_KEY_V       ALLEGRO_KEY_V
#define KYB_KEY_W       ALLEGRO_KEY_W
#define KYB_KEY_X       ALLEGRO_KEY_X
#define KYB_KEY_Y       ALLEGRO_KEY_Y
#define KYB_KEY_Z       ALLEGRO_KEY_Z

/**
 *  Riceve informazioni sullo stato della tastiera e sui tasti premuti
 */
void keyboard_get_state(void);

/**
 *  Verifica che un tasto sia premuto
 *
 *  @param key Tasto
 *
 *  @retval true Tasto premuto
 *  @retval false Tasto non premuto
 */
int keyboard_is_key_down(int key);

/**
 *  Converte un carattere compreso tra A e Z nell'identificativo del tasto corrispondente
 *
 *  @param asc Carattere
 *
 *  @return Identificativo del tasto
 *  @retval 0 se il carattere è fuori dall'intervallo [A, Z]
 */
int keyboard_ascii_to_key(int asc);

#endif
