#include <lcom/lcf.h>
#include <lcom/timer.h>
#include <stdint.h>
#include "i8254.h"
#include <minix/syslib.h>

static int hook_id = 0;
uint32_t global_counter = 0;


uint32_t timer_get_count() {
  return global_counter; 
}

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  uint8_t st;
  
  // ler o status byte
  if(timer_get_conf(timer, &st) != 0) {
    return -1;
  }

  uint8_t control_word = (timer << 6 ) | TIMER_LSB_MSB  | (st & 0x0F);

  if (sys_outb(TIMER_CTRL, control_word) != 0) {
    return -1;
  }

  // dividerr = timerFreq/desiredFReq
  uint16_t val = (uint16_t)(TIMER_FREQ / freq);

  // dividir em lsb e msb
  uint8_t lsb, msb;
  util_get_LSB(val, &lsb);
  util_get_MSB(val, &msb);

  uint8_t count_register; 
  switch (timer)
  {
  case 0:
    count_register = TIMER_0;
    break;
  case 1: 
    count_register = TIMER_1;
    break; 
  case 2: 
    count_register = TIMER_2;
    break;

  default:
    return -1; 
  }

  if (sys_outb(count_register, lsb) != 0) return -1;
  if (sys_outb(count_register, msb) != 0) return -1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  *bit_no = (uint8_t) hook_id; 
  sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id);

  return 0;
}

int (timer_unsubscribe_int)() {
  sys_irqrmpolicy(&hook_id);
  return 0;
}

void (timer_int_handler)() {
  global_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  // timer whose configuration to read ranges from 0 to 2; 
  // Adress fo memory position to be filled with the timer cofig (status byte); 
  // returns 0 on sucess or -1 in failure; 

  // bit 7 e 6
  uint8_t read_back = 0;
  read_back = TIMER_RB_COUNT_ | TIMER_RB_CMD | TIMER_RB_SEL(timer);

  if (sys_outb(TIMER_CTRL, read_back) != 0) {
    printf("Error: failed to write read-back command to control register\n");
  } 

  uint8_t count_register; 
  switch (timer)
  {
  case 0:
    count_register = TIMER_0;
    break;
  case 1: 
    count_register = TIMER_1;
    break; 
  case 2: 
    count_register = TIMER_2;
    break;

  default:
    return 1; 
  }


  if (util_sys_inb(count_register, st) != 0) {
    printf("Error: failed to read status from the specified timer register\n");
    return 1 ; 
  }
  
  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {
                          
  /* union timer_status_field_val {
  uint8_t byte;            !< status 
  enum timer_init in_mode; !< initialization mode 
  uint8_t count_mode;      !< counting mode: 0, 1,.., 5 
  bool bcd;                !< counting base, true if BCD 
  };*/
  union timer_status_field_val conf;


  /* enum timer_status_field {
  tsf_all,     !< configuration/status 
  tsf_initial, !< timer initialization mode 
  tsf_mode,    !< timer counting mode 
  tsf_base     !< timer counting base 
  };*/

  switch (field)
  {
  case (tsf_all):
    conf.byte = st;  
    break;
  
  case (tsf_initial):
    /*enum timer_init {
    INVAL_val,    !< Invalid initialization mode 
    LSB_only,     !< Initialization only of the LSB 
    MSB_only,     !< Initialization only of the MSB 
    MSB_after_LSB !< Initialization of LSB and MSB, in this order 
    */
    conf.in_mode = (st & (BIT(4) | BIT(5))) >> 4;
    break;
  
  case (tsf_mode):
    conf.count_mode = (st & (BIT(3) | BIT(2) | BIT(1))) >> 1;
    break;
  
  
  case (tsf_base): 
    conf.bcd = st & BIT(0);
    break;
  
  default: // tsf_base  
    return 1;
}
  

  timer_print_config(timer, field, conf);
  return 0;
}
