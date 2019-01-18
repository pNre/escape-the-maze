#include <stdlib.h>
#include <stdint.h>
#include "misc/random.h"

/* initialize state to random bits  */
static unsigned long state[16];
/* init should also reset this to 0 */
static unsigned int well_index = 0;

void well512_initialize(void)
{
    unsigned long j;
    
    srand((unsigned int) time(NULL));
    for (j = 0; j < sizeof(state) / sizeof(unsigned long); j++)
        state[j] = (unsigned long)rand();
    return;
}

unsigned long well512_random(void)
{
    unsigned long a, b, c, d;
    a = state[well_index];
    c = state[(well_index+13)&15];
    b = a^c^(a<<16)^(c<<15);
    c = state[(well_index+9)&15];
    c ^= (c>>11);
    a = state[well_index] = b^c;
    d = a^((a<<5)&0xDA442D24UL);
    well_index = (well_index + 15)&15;
    a = state[well_index];
    state[well_index] = a^b^d^(a<<2)^(b<<18)^(c<<28); return state[well_index];
}

char * random_string(size_t max_length)
{
    
    max_length = max_length < 1 ? 1 : max_length;
    
    int length = random_int(1, max_length);
    
    char * string = memalloc(char, length + 1);
    
    int i;
    for (i = 0; i < length; i++) {
        string[i] = random_bool(0.5) ? random_int(65, 90) : random_int(97, 122);
    }
    
    string[i] = 0;
    
    return string;
    
}
