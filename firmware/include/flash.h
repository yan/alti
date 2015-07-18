/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __ADESTO_FLASH_H
#define __ADESTO_FLASH_H

#include <stdint.h>
#include <stddef.h>

/* Read commands, Table 15-1 in specs */
#define ADESTO_READ_MAIN_MEM_PAGE                   0xD2
#define ADESTO_READ_CONTINUOUS_ARR_READ_LP          0x01
#define ADESTO_READ_CONTINUOUS_ARR_READ_LF          0x03
#define ADESTO_READ_CONTINUOUS_ARR_READ_HF1         0x0B
#define ADESTO_READ_CONTINUOUS_ARR_READ_HF2         0x1B
#define ADESTO_READ_BUFFER_1_LF                     0xD1
#define ADESTO_READ_BUFFER_2_LF                     0xD3
#define ADESTO_READ_BUFFER_1_HF                     0xD4
#define ADESTO_READ_BUFFER_2_HF                     0xD6

/* Program and Erase Commands */
#define ADESTO_WRITE_BUFFER_1                       0x84
#define ADESTO_WRITE_BUFFER_2                       0x87
#define ADESTO_WRITE_BUFFER_1_TO_MEM_W_ER           0x83
#define ADESTO_WRITE_BUFFER_2_TO_MEM_W_ER           0x86
#define ADESTO_WRITE_BUFFER_1_TO_MEM_WO_ER          0x88
#define ADESTO_WRITE_BUFFER_2_TO_MEM_WO_ER          0x89
#define ADESTO_WRITE_MEM_THROUGH_BUF1_W_ER          0x82
#define ADESTO_WRITE_MEM_THROUGH_BUF2_W_ER          0x85
#define ADESTO_WRITE_BYTE_PAGE_THROUGH_BUF1_WO_ER   0x02
#define ADESTO_WRITE_PAGE_ERASE                     0x81
#define ADESTO_WRITE_BLOCK_ERASE                    0x50
#define ADESTO_WRITE_SECTOR_ERASE                   0x7C
#define ADESTO_WRITE_CHIP_ERASE_CMD1                0xC7
#define ADESTO_WRITE_CHIP_ERASE_CMD2                0x94
#define ADESTO_WRITE_CHIP_ERASE_CMD3                0x80
#define ADESTO_WRITE_CHIP_ERASE_CMD4                0x9A
#define ADESTO_WRITE_PROGRAM_SUSPEND                0xB0
#define ADESTO_WRITE_PROGRAM_RESUME                 0xD0

/* Protection and Security Commands */
/* TODO */

/* Additional Commands */
#define ADESTO_AUX_MEM_TO_BUF1_XFER                 0x53
#define ADESTO_AUX_MEM_TO_BUF2_XFER                 0x55
#define ADESTO_AUX_MEM_TO_BUF1_CMP                  0x60
#define ADESTO_AUX_MEM_TO_BUF2_CMP                  0x61
#define ADESTO_AUX_AUTO_REWRITE_BUF_1               0x58
#define ADESTO_AUX_AUTO_REWRITE_BUF_2               0x59
#define ADESTO_AUX_DEEP_POWER_DOWN                  0xB9
#define ADESTO_AUX_RESUME_DEEP_POWER_DOWN           0xAB
#define ADESTO_AUX_ULTRA_DEEP_POWER_DOWN            0x79
#define ADESTO_AUX_STATUS_REGISTER_READ             0xD7
#define ADESTO_AUX_MFGR_DEVICE_READ                 0x9F


void config_flash(void);
int test_flash(void);
void flash_read(uint32_t addr, uint8_t *data, size_t size);
void flash_write(uint32_t addr, uint8_t *data, size_t size);


#endif /* __ADESTO_FLASH_H */
