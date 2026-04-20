#include <lcom/lcf.h>
#include "kbc.h"

uint8_t scancode_byte = 0;
int kbd_hook_id = 1;

uint8_t get_scancode_byte() {
  return scancode_byte; 
}

int (kbd_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t) kbd_hook_id;
  return sys_irqsetpolicy(1, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbd_hook_id);
}

int (kbd_unsubscribe_int)() {
  return sys_irqrmpolicy(&kbd_hook_id);
}

// interrupt handler do teclado
void (kbc_ih)() {
  uint8_t status;
  // ler o status do kbc para perceber ver se temos scancode para ler e se nao ha erros
  if (util_sys_inb(ST_REG, &status) != 0) return;

  if (status & KBC_OBF) {
    uint8_t data;
    if (util_sys_inb(OUT_BUFFER, &data) != 0) return;

    // erros de parity e timeout
    if (!(status & (KBC_PARITY | KBC_TIMEOUT))) {
      scancode_byte = data;
    }
  }
}