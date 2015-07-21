/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <stdint.h>
#include <math.h>
#include <hal.h>
#include <spi.h>

#include <ms5611.h>
#include <globals.h>
#include <filter.h>
#include <util.h>
#include <pins.h>

#define USE_SPI

/**
 * @brief PROM
 */
typedef uint16_t coefficient_t;

/**
 * @brief PROM
 */
struct ms5611_C_s {
  union {
    struct {
      coefficient_t C0_reserved;
      coefficient_t C1_pressure_sensitivity;
      coefficient_t C2_pressure_offset;
      coefficient_t C3_temp_press_sensitivity;
      coefficient_t C4_temp_press_offset;
      coefficient_t C5_ref_temp;
      coefficient_t C6_temp_coefficient_temp;
      coefficient_t C7_crc;
    };
    uint16_t _C[8];
  };
} __attribute__((packed)) C;

static int32_t _temp;

static uint32_t ms5611_read_adc(uint8_t cmd);


#if MS5611_VERIFY_RECVD == 1

/**
 * @brief Verify that the calibration PROM we received from the MS5611
 *  was received successfully
 */
uint16_t ms5611_verify_prom(void)
{
  int16_t cnt; // simple counter
  uint16_t n_rem; // crc reminder
  uint16_t crc_read; // original value of the crc
  uint8_t n_bit;
  uint16_t *n_prom = (uint16_t*)C._C;

  n_rem = 0x00;
  crc_read = n_prom[7]; //save read CRC
  n_prom[7] = (0xFF00 & (n_prom[7])); //CRC byte is replaced by 0

  for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
  { // choose LSB or MSB
    if (cnt%2==1)
      n_rem ^= (unsigned short) ((n_prom[cnt>>1]) & 0x00FF);
    else
      n_rem ^= (unsigned short) (n_prom[cnt>>1]>>8);
    for (n_bit = 8; n_bit > 0; n_bit--) {
      if (n_rem & (0x8000)) {
        n_rem = (n_rem << 1) ^ 0x3000;
      } else {
        n_rem = (n_rem << 1);
      }
    }
  }
  n_rem= (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code
  n_prom[7]=crc_read; // restore the crc_read to its original place
  return (n_rem ^ 0x00);
}

#endif

/**
 * @brief Reset the MS5611.
 */
void ms5611_reset(void)
{
#if defined(USE_SPI)
  spi_send_msb_first(MS5611_PORT);
#endif

  pin_clear(MS5611_GPIO, MS5611_EN);
  arch_spi_xfer(MS5611_PORT, MS5611_CMD_RESET);

  /* Give it 3ms to start */
  delay_ms(3);

  pin_set(MS5611_GPIO, MS5611_EN);

}

static uint16_t ms5611_get16(uint8_t cmd)
{
  uint16_t val;

#if defined(USE_SPI)
  spi_send_msb_first(MS5611_PORT);
#endif

  pin_clear(MS5611_GPIO, MS5611_EN);
  arch_spi_xfer(MS5611_PORT, cmd);
  val = spi_read_octets(MS5611_PORT, 2, BYTEORDER_MSB);
  pin_set(MS5611_GPIO, MS5611_EN);

  return val;
}

/**
 * @brief Initialize the MS5611 sensor, get its PROM contents.
 */
void ms5611_init(void)
{
  uint8_t cmd, idx;
#if MS5611_VERIFY_RECVD == 1
  uint16_t crc4_dword;
  uint8_t crc;
#endif

  ms5611_reset();

  /* Skip over the reserved */
  for ( cmd  = MS5611_CMD_PROM_READ_BASE, idx = 0;
        cmd <= MS5611_CMD_PROM_READ_LAST;
        cmd += sizeof(coefficient_t), idx++) {
    C._C[idx] = ms5611_get16(cmd);
  }

  filter_init_state(&g.baro_filter_state);

#if MS5611_VERIFY_RECVD == 1
  arch_spi_xfer(MS5611_PORT, MS5611_CMD_PROM_READ_LAST);
  crc4_dword = spi_read_octets(MS5611_PORT, 2, BYTEORDER_MSB);

  crc = ms5611_verify_prom();
  
  if ((crc4_dword & 0x0F) != crc) {
    // Do something here.. shrug
  }
#endif
}

/**
 * @brief Get the value from one of two MS5611's ADCs. The first is temperature,
 * second is pressure.
 */
static uint32_t ms5611_read_adc(uint8_t cmd)
{
  uint32_t val;

#if defined(USE_SPI)
  spi_send_msb_first(MS5611_PORT);
#endif

  /* Warn that we're about to read, and wait a period of time depending on the
   * precision required*/
  pin_clear(MS5611_GPIO, MS5611_EN);
  arch_spi_xfer(MS5611_PORT, cmd);
  delay_ms((cmd&0x0F) * 2 + 1);
  pin_set(MS5611_GPIO, MS5611_EN);

  /* Read back the ADC result */
  pin_clear(MS5611_GPIO, MS5611_EN);
  arch_spi_xfer(MS5611_PORT, MS5611_CMD_ADC_READ);
  val = spi_read_octets(MS5611_PORT, 3, BYTEORDER_MSB);
  pin_set(MS5611_GPIO, MS5611_EN);

  return val;
}

/**
 * @brief Return the temperature from MS5611 as (deg_C * 100). Generally not
 * used on its own as it reads the ADC an extra time. Use ms5611_get_last_temp()
 * if also getting pressure.
 */
uint32_t ms5611_get_temp(uint8_t precision)
{
  int64_t dT, D2, T;

  assert(precision <= 4);

  D2 = ms5611_read_adc(MS5611_CMD_D2_BASE + precision * 2);
  dT = D2 - (C.C5_ref_temp << 8);
  T = 2000 + ((dT * C.C6_temp_coefficient_temp) >> 23);

#if MS5611_USE_COMPENSATION
  if (T < 2000) {
    T -= (dT * dT) >> 31;
  }
#endif

  return (uint32_t)T;
}

/**
 * @brief Return the temperature from the last ADC reading.
 */
uint32_t ms5611_get_last_temp()
{
  return _temp;
}

/**
 * @brief Return the pressure from MS5611 as (mbar * 100).
 */
uint32_t ms5611_get_mbarc(uint8_t precision)
{
  uint32_t D1, D2;
  int64_t dT, P, off, sens;
  typedef int64_t i64;
  int32_t saved_temp;

  assert(precision <= 4);

  D1 = ms5611_read_adc(MS5611_CMD_D1_BASE + precision * 2);
  D2 = ms5611_read_adc(MS5611_CMD_D2_BASE + precision * 2);

  dT = D2 - ((i64)C.C5_ref_temp << 8);
  off =  ((i64)C.C2_pressure_offset      << 16) + ((dT * (i64)C.C4_temp_press_offset) >> 7);
  sens = ((i64)C.C1_pressure_sensitivity << 15) + ((dT * (i64)C.C3_temp_press_sensitivity) >> 8);

  _temp = ((dT * (i64)C.C6_temp_coefficient_temp) >> 23);
  saved_temp = _temp;

#if MS5611_USE_COMPENSATION
  {
    int32_t off2 = 0, sens2 = 0;

    if (_temp < 0) {
      int32_t temp_sq = _temp * _temp;
      _temp -= (dT * dT) >> 31;
      off2 = 5 * (temp_sq >> 1);
      sens2 = 5 * (temp_sq >> 2);
    }

    if (_temp < -3500) {
      int32_t temp_low_sq = (saved_temp + 3500) * (saved_temp + 3500);
      off2 += 7 * temp_low_sq;
      sens2 += 11 * (temp_low_sq >> 1);
    }

    _temp += 2000;
    off -= off2;
    sens -= sens2;
  }
#endif

  
  P = ((D1 * (sens >> 21) - off) >> 15);

  return filter_add_value(&g.baro_filter_state, P);
}

