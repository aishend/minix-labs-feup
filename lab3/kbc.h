#ifndef _LCOM_KBC_H_
#define _LCOM_KBC_H_

#include <lcom/lcf.h>

#define OUT_BUFFER 0x60 
#define IN_BUFFER  0x60 
#define ST_REG     0x64 
#define CMD_REG    0x64 

#define KBC_PARITY    BIT(7)
#define KBC_TIMEOUT   BIT(6)
#define KBC_IBF       BIT(1)
#define KBC_OBF       BIT(0)

#define READ_CMD_BYTE  0x20
#define WRITE_CMD_BYTE 0x60
#define KBC_INT_BIT    BIT(0)

#define ESC_BREAK_CODE 0x81
#define DELAY_US       20000

int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
void (kbc_ih)();
int (kbc_read_value)(uint8_t *value);
int (kbc_write_command)(int port, uint8_t cmd);
int (kbc_enable_interrupts)();
void (timer_int_handler)(); 

extern uint32_t global_counter;

#endif