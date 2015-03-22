/**
 *
 *
 *
 */

#ifndef __MS5611_H
#define __MS5611_H

#include <stdint.h>

#include "i2c.h"

#define MS5611_CMD_RESET          0x1E
#define MS5611_CMD_D1_BASE        0x40
#define MS5611_CMD_D2_BASE        0x50

#define MS5611_CMD_ADC_READ       0x00
#define MS5611_CMD_PROM_READ_BASE 0xA0
#define MS5611_CMD_PROM_READ_LAST 0xAE

#define MS5611_ADDR               0x77

/**
 * Set to 1 to enable second-order compensation
 */
#define MS5611_USE_COMPENSATION   1

/**
 * If set to 1, verify the PROM data received from MS5611 on initialization
 * using crc4.
 */
#define MS5611_VERIFY_RECVD       1

/**
 *
 */
#define MS5611_PROM_CRC_MASK      0x0F

/**
 * @brief
 */
void ms5611_init(uint32_t i2c);

/**
 * @brief
 */
uint32_t ms5611_get_temp(uint32_t i2c, uint8_t precision);

/**
 * @brief
 */
uint32_t ms5611_get_last_temp(void);

/**
 * @brief
 */
uint32_t ms5611_get_mbarc(uint32_t i2c, uint8_t precision);

/**
 * @brief
 */
void ms5611_reset(uint32_t i2c);


#if MS5611_VERIFY_RECVD == 1
/**
 *
 */
uint16_t ms5611_verify_prom(void);
#endif

#endif
