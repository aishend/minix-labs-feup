#include "rtc.h"

#include <minix/syslib.h>
#include <minix/sysutil.h>

#define TODO return -1

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_DAY 0x07
#define RTC_REG_MONTH 0x08
#define RTC_REG_YEAR 0x09
#define RTC_UIP_MSK (1 << 7)
#define RTC_DM_MSK (1 << 2)

static int bcd_to_bin(uint8_t bcd) {
  return ((bcd >> 4) * 10) + (bcd & 0x0F); 
; }


int rtc_read_date(rtc_date *date) { 
  sys_outb(RTC_ADDR_REG, RTC_REG_A);

  uint32_t rtcRead = 0;
  sys_inb(RTC_DATA_REG, &rtcRead);
  
  // obter apenas o digito mais significativo

  uint8_t rtcUip = rtcRead & RTC_UIP_MSK; 
  while(rtcUip != 0) {
    tickdelay(micros_to_ticks(10));
    sys_outb(RTC_ADDR_REG, RTC_REG_A);
    sys_inb(RTC_DATA_REG, &rtcRead);
    rtcUip = rtcRead & RTC_UIP_MSK;  ;
  }

  uint32_t day, month, year;

  sys_outb(RTC_ADDR_REG, RTC_REG_DAY);
  sys_inb(RTC_DATA_REG, &day);
  sys_outb(RTC_ADDR_REG, RTC_REG_MONTH);
  sys_inb(RTC_DATA_REG, &month);

  
  sys_outb(RTC_ADDR_REG, RTC_REG_YEAR);
  sys_inb(RTC_DATA_REG, &year);

  // VER SE OS DADSO ESTAO EM BCD
  sys_outb(RTC_ADDR_REG, RTC_REG_B);
  uint32_t rtc_b_register;
  sys_inb(RTC_DATA_REG, &rtc_b_register);

  uint8_t dm_bit = rtc_b_register & RTC_DM_MSK;


  // se os dados estiverem em bcd temos de converte-los 
  if(!dm_bit) {
    day = bcd_to_bin(day);
    month = bcd_to_bin(month);
    year = bcd_to_bin(year);
  }

  date->day = day;
  date->month = month; 
  date->year = year; 

  printf("Data lida do RTC: %02d/%02d/20%02d\n", date->day, date->month, date->year);
  return 0; 

}
