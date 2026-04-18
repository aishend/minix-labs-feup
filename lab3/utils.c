#include <lcom/lcf.h>
#include <stdint.h>
#include "kbc.h"

int (util_sys_inb)(int port, uint8_t *value) {
    uint32_t temp_val;
    if (sys_inb(port, &temp_val) != 0) return 1;
    *value = (uint8_t) temp_val;
    return 0;
}

int (kbc_read_value)(uint8_t *value) {
    uint8_t status;
    
    if (util_sys_inb(ST_REG, &status) != 0) return 1;

    if (status & KBC_OBF) {
        return util_sys_inb(OUT_BUFFER, value);
    }
    return 1;
}


int (kbc_enable_interrupts)() {
    uint8_t cmd_byte;

    if (sys_outb(ST_REG, READ_CMD_BYTE) != 0) return 1;
    if (util_sys_inb(OUT_BUFFER, &cmd_byte) != 0) return 1;

    cmd_byte |= KBC_INT_BIT;

    if (sys_outb(ST_REG, WRITE_CMD_BYTE) != 0) return 1;
    return sys_outb(OUT_BUFFER, cmd_byte);
}