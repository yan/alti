
#ifndef __I2C_H
#define __I2C_H


void     i2c_enable(void);
void     i2c_config(uint32_t i2c);
void     i2c_disable(void);
void     i2c_send_buf(uint32_t i2c, uint8_t address, uint8_t *buf, uint32_t length);
void     i2c_read_data(uint32_t i2c, uint8_t address, uint8_t *data, uint32_t length);
uint32_t i2c_read_octets(uint32_t i2c, uint8_t address, unsigned octets);
void     i2c_send_cmd(uint32_t i2c, uint8_t address, uint8_t cmd);

#define i2c_read16(i2c, addr) (uint16_t)i2c_read_octets(i2c, addr, 2);
#define i2c_read24(i2c, addr) (uint32_t)i2c_read_octets(i2c, addr, 3);
#define i2c_read32(i2c, addr) (uint32_t)i2c_read_octets(i2c, addr, 4);

#endif
