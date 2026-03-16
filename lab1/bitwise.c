#include "bitwise.h"

#define TODO return 255

uint8_t clear(uint8_t msk, int pos) {
  uint8_t mascara = (1 << pos);
  return msk & (~mascara);
}

uint8_t set(uint8_t msk, int pos) {
  uint8_t mascara = (1 << pos);
  return msk | mascara;
}

bool is_set(uint8_t msk, int pos) { 
  uint8_t mascara  = (1 << pos);
  return (msk & mascara) ;
}

uint8_t lsb(uint16_t wide_msk) { 
  uint16_t mascara = 0x00FF;
  return (uint8_t) (wide_msk & mascara);
}

uint8_t msb(uint16_t wide_msk) {
  uint16_t mascara = 0x00FF;
  wide_msk = wide_msk >> 8;
  return (uint8_t )(wide_msk & mascara);
}

#include <stdarg.h>
uint8_t mask(int pos, ...) { 
  va_list ap; 
  va_start(ap ,pos);
  uint8_t mascara = set(0, pos);
  int atual = va_arg(ap, int); 

  while( atual != MSK_END) {
    mascara = set(mascara, atual);
    atual = va_arg(ap, int); 
  }
  va_end(ap);
  return mascara;
}
