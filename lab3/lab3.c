#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "kbc.h"

uint8_t get_scancode_byte(); 

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {


  /*
    make: indicates whether this is a makecode or a breakcode
    size: number of bytes of the scancode
    bytes: is an array containing the bytes of the scancode read from the KBC. The array should contain the bytes in the order they are read from the KBC.
   */
  int ipc_status, r;
  message msg;
  uint8_t bit_no;
  kbd_subscribe_int(&bit_no); 
  uint32_t irq_set = BIT(bit_no);
  uint8_t bytes[2];
  uint8_t size = 0;

  while (1) {
    // 
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        if (msg.m_notify.interrupts & irq_set) {
          kbc_ih();
          uint8_t b = get_scancode_byte();
          bytes[size++] = b;
          if (b == 0xE0) continue;
          kbd_print_scancode(!(b & BIT(7)), size, bytes);
          if (b == ESC_BREAK_CODE) {
            kbd_unsubscribe_int();
            return 0;
          }
          size = 0;
        }
      }
    }
  }
  return 1;
}

// sem usar interrupcoes
int (kbd_test_poll)() {
  uint8_t bytes[2], data;
  uint8_t size = 0;
  // 
  while (1) {
    // ler em loop o valor do kbc e verificar se tem scancode para ler
    if (kbc_read_value(&data) == 0) {
      bytes[size++] = data;
      if (data == 0xE0) continue;
      kbd_print_scancode(!(data & BIT(7)), size, bytes);
      if (data == ESC_BREAK_CODE) break;
      size = 0;
    }
  }
  // reativar interrupts pq o poll desativa-os
  return kbc_enable_interrupts();
}

int (kbd_test_timed_scan)(uint8_t n) {
  int ipc_status, r;
  message msg;
  uint8_t kbd_bit, timer_bit;
  uint8_t bytes[2], size = 0;

  // subscrever os dois ao mesmo tempo
  kbd_subscribe_int(&kbd_bit);
  timer_subscribe_int(&timer_bit);
  uint32_t kbd_irq = BIT(kbd_bit);
  uint32_t timer_irq = BIT(timer_bit);

  while (1) {
    // esperamos por mensagens de interrupcao do timer ou do teclado
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) continue;
    if (is_ipc_notify(ipc_status)) {
      if (_ENDPOINT_P(msg.m_source) == HARDWARE) {
        // interrupcao do timer
        if (msg.m_notify.interrupts & timer_irq) {
          timer_int_handler(); //incrementa o global counter
          if (global_counter >= (uint32_t)(n * 60)) {
            kbd_unsubscribe_int();
            timer_unsubscribe_int();
            return 0;
          }
        }

        // interrupcao do teclado
        if (msg.m_notify.interrupts & kbd_irq) {
          kbc_ih();
          uint8_t b = get_scancode_byte();
          // dar reset a variavel que conta o tempo pq houve atividade do teclado
          global_counter = 0; 
          // guardar o byte lido do kbc
          bytes[size++] = b;

          // se isto acontecer significa uqe temos de esperar mais um byte para ler o scancode completo
          if (b == 0xE0) continue;
          kbd_print_scancode(!(b & BIT(7)), size, bytes);
          if (b == ESC_BREAK_CODE) {
            // ja nao precisamos de tratar mais das interrupcoews logo damos unsubscribe do timer e do kbc
            kbd_unsubscribe_int();
            timer_unsubscribe_int();
            return 0;
          }
          size = 0;
        }
      }
    }
  }
  return 1;
}