#include "bitwise.h"
#include <stdarg.h>

#define TODO return 255

uint8_t clear(uint8_t msk, int pos) {  
    // return uint8_t with bit at pos = 0
    return msk & ~(1 << pos);
}

uint8_t set(uint8_t msk, int pos) { 
    // return uint8_t with bit at pos = 1
    return msk |= (1 << pos);
}

bool is_set(uint8_t msk, int pos) { 
    // return boolean saying if bit at pos is 1
    return msk & (1 << pos);
}

uint8_t lsb(uint16_t wide_msk) { 
    // return least significant byte
    // return (uint8_t)(wide_msk & 0xFF); <-- this is best practice but not needed

    return (uint8_t)(wide_msk);
}

uint8_t msb(uint16_t wide_msk) { 
    // return most significant byte
    // return (uint8_t)((wide_msk >> 8) & 0xFF); <-- same as above

    return (uint8_t)(wide_msk >> 8);
}

uint8_t mask(int pos, ...) { 
    uint8_t result = 0;
    va_list args; // 

    // this initializes the pointer AFTER pos
    va_start(args, pos);

    // because of the above, we need to set the pos bit manually
    /* result |= (1 << pos); set() does the same */
    result = set(result, pos);

    // get the first element
    int next_pos = va_arg(args, int);
    while (next_pos != MSK_END) {
      /* result |= (1 << next_pos); === set() does the same === */ 
      result = set(result, next_pos);

      // go to the next one
      next_pos = va_arg(args, int);
    }

    // clean up
    va_end(args);

    return result;

}
