#include "io/keyboard.h"

ALLEGRO_KEYBOARD_STATE keyState;

void keyboard_get_state(void)
{
    al_get_keyboard_state(&keyState);
}

int keyboard_is_key_down(int key)
{
    return al_key_down(&keyState, key);
}

int keyboard_ascii_to_key(int asc)
{
    const int ascii_to_key_map[] = {
        KYB_KEY_A,
        KYB_KEY_B,
        KYB_KEY_C,
        KYB_KEY_D,
        KYB_KEY_E,
        KYB_KEY_F,
        KYB_KEY_G,
        KYB_KEY_H,
        KYB_KEY_I,
        KYB_KEY_J,
        KYB_KEY_K,
        KYB_KEY_L,
        KYB_KEY_M,
        KYB_KEY_N,
        KYB_KEY_O,
        KYB_KEY_P,
        KYB_KEY_Q,
        KYB_KEY_R,
        KYB_KEY_S,
        KYB_KEY_T,
        KYB_KEY_U,
        KYB_KEY_V,
        KYB_KEY_W,
        KYB_KEY_X,
        KYB_KEY_Y,
        KYB_KEY_Z
    };
    
    if (asc >= 'A' && asc <= 'Z')
        return ascii_to_key_map[asc - 'A'];

    return 0;

}
