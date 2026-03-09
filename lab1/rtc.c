#include "rtc.h"
#include <minix/syslib.h>
#include <lcom/lcf.h>

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

  // in this case, a uint8_t being in BCD format means that the left 4 bits are for the most significant digit and the right 4 bits are for the least significant digit

  // in order to convert:
  // (bcd >> 4) * 10 gets the tens
  // bcd & 0x0F gets the units

  // sum up and voila

  return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

int rtc_read_date(rtc_date *date) {
  uint32_t reg_A, reg_B;

  // variables to check return values
  int retv;

  /* ==== CHECKING IF THE DATA IS BCD ==== */

  // select the 0x0B register
  if ((retv = sys_outb(RTC_ADDR_REG, RTC_REG_B)) != 0) return retv;

  // read from it
  if ((retv = sys_inb(RTC_DATA_REG, &reg_B)) != 0) return retv;

  // check if is in binary format (or BCD) 
  int is_binary = reg_B & RTC_DM_MSK;

  /* ==== CHECKING IF RTC IS READY TO READ ==== */

  // select the 0x0A register
  if ((retv = sys_outb(RTC_ADDR_REG, RTC_REG_A)) != 0) return retv; 

  // read from it
  if ((retv = sys_inb(RTC_DATA_REG, &reg_A)) != 0) return retv; 

  // this loop will only break once the RTC stops updating
  while (reg_A & RTC_UIP_MSK) {
    // delay some time
    tickdelay(micros_to_ticks(1000)); // delay for 1000 micro-seconds

    // keep checking if the RTC is updating
    if ((retv = sys_outb(RTC_ADDR_REG, RTC_REG_A)) != 0) return retv; 
    if ((retv = sys_inb(RTC_DATA_REG, &reg_A)) != 0) return retv;
  }

  /* ==== at this point RTC should have finished updating its data, so we can proceed to reading the date ==== */

  // sys_inb writes into uint32_t
  uint32_t day_read, month_read, year_read;

  // read the day
  if ((retv = sys_outb(RTC_ADDR_REG, RTC_REG_DAY)) != 0) return retv;
  if ((retv = sys_inb(RTC_DATA_REG, &day_read)) != 0) return retv;

  // read the month
  if ((retv = sys_outb(RTC_ADDR_REG, RTC_REG_MONTH)) != 0) return retv;
  if ((retv = sys_inb(RTC_DATA_REG, &month_read)) != 0) return retv;

  // read the year
  if ((retv = sys_outb(RTC_ADDR_REG, RTC_REG_YEAR)) != 0) return retv;
  if ((retv = sys_inb(RTC_DATA_REG, &year_read)) != 0) return retv;

  // convert to uint8_t
  day_read = (uint8_t) day_read; 
  month_read = (uint8_t) month_read;
  year_read = (uint8_t) year_read;
  
  // convert from BCD if necessary
  if (!is_binary) {
    day_read = bcd_to_bin(day_read);
    month_read = bcd_to_bin(month_read);
    year_read = bcd_to_bin(year_read);
  }

  // save in rtc_date struct
  date->day = day_read;
  date->month = month_read;
  date->year = year_read;

  return 0;
}

// sys_inb() ---> read from a port
// sys_outb() ---> write into a port

// RTC
//  bit 7 (UIP) if 1 ---> update happening
//              if 0 ---> update finisehd - safe to read

// ADDRESS_REG = 0x70 -> write
// DATA_REG = 0x71 -> read
// STATUS_REG = from 0x0A to 0x0D
//                     |
//                     |
//                     ---> UIP;
// 0x0B ---> Data Mode