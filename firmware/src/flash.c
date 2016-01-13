/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <string.h>

#include <flash.h>
#include <pins.h>
#include <hal.h>
#include <globals.h>
#include <util.h>
#include <spi.h>


struct status_register_s {
  uint8_t page_size_config : 1;
  uint8_t sector_protect_status : 1;
  uint8_t density_code : 4;
  uint8_t cmp_result : 1;
  uint8_t ready : 1;

  uint8_t erase_suspend : 1;
  uint8_t suspend_status_b1 : 1;
  uint8_t suspend_status_b2 : 1;
  uint8_t sector_lockdown : 1;
  uint8_t _reserved : 1;
  uint8_t error : 1;
  uint8_t reserved : 1;
  uint8_t _ready : 1;
};

inline static void flash_reset(void);
static void busy_wait_for_ready(void);
static void flash_write_buffer(uint8_t *data, size_t size);
static void flash_commit_buffer(uint32_t address);
static void flash_read_status(struct status_register_s *dest);

inline static void flash_reset(void)
{
  int i = 0;

  pin_set(ADESTO_FLASH_RESET_GPIO, ADESTO_FLASH_RESET);
  pin_clear(ADESTO_FLASH_RESET_GPIO, ADESTO_FLASH_RESET);
  for (i = 0; i < 30; i++)
    ;
  pin_set(ADESTO_FLASH_RESET_GPIO, ADESTO_FLASH_RESET);
}

static void flash_write_buffer(uint8_t *data, size_t size)
{
  pin_clear(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);

  /* First, write it to the flash buffer */
  arch_spi_xfer(ADESTO_FLASH_BUS, ADESTO_WRITE_BUFFER_2);

  /* Send 15 dummy bits, then 9 intra-sector offset. */
  arch_spi_xfer(ADESTO_FLASH_BUS, 0);
  arch_spi_xfer(ADESTO_FLASH_BUS, 0);
  arch_spi_xfer(ADESTO_FLASH_BUS, 0);

  /* Then, clock in the actual data */
  spi_send_buf(ADESTO_FLASH_BUS, data, size);

  /* And we're done */
  pin_set(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);
}

static void flash_commit_buffer(uint32_t address)
{
  pin_clear(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);

  arch_spi_xfer(ADESTO_FLASH_BUS, ADESTO_WRITE_BUFFER_2_TO_MEM_W_ER);
  arch_spi_xfer(ADESTO_FLASH_BUS, (address & 0xFF0000) >> 16);
  arch_spi_xfer(ADESTO_FLASH_BUS, (address & 0xFF00) >> 8);
  arch_spi_xfer(ADESTO_FLASH_BUS,  address & 0xFF);
  
  pin_set(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);
}

static void flash_read_status(struct status_register_s *dest)
{
  spi_set_msb(ADESTO_FLASH_BUS);

  pin_clear(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);

  uint16_t returned;

  arch_spi_xfer(ADESTO_FLASH_BUS, ADESTO_AUX_STATUS_REGISTER_READ);

  returned = spi_read_octets(ADESTO_FLASH_BUS, sizeof(*dest), BYTEORDER_MSB);

  returned = (returned & 0x00ff) << 8 | (returned & 0xff00) >> 8;

  * (uint16_t*) dest = returned;

  pin_set(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);

}
static void busy_wait_for_ready(void)
{
  struct status_register_s status_reg;

  status_reg.ready = 0;

  while (!status_reg.ready) {
    flash_read_status(&status_reg);
  }
}

void flash_read(uint32_t addr, uint8_t *data, size_t size)
{
  spi_set_msb(ADESTO_FLASH_BUS);

  pin_clear(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);

  /* Read memory in low power mode */
  arch_spi_xfer(ADESTO_FLASH_BUS, ADESTO_READ_CONTINUOUS_ARR_READ_LP);

  /* Send the address */
  arch_spi_xfer(ADESTO_FLASH_BUS, (addr & 0xFF0000) >> 16);
  arch_spi_xfer(ADESTO_FLASH_BUS, (addr & 0xFF00) >> 8);
  arch_spi_xfer(ADESTO_FLASH_BUS,  addr & 0xFF);

  /* Then read as much as we asked for */
  spi_recv_buf(ADESTO_FLASH_BUS, data, size);

  pin_set(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);
}

void flash_write(uint32_t addr, uint8_t *data, size_t size)
{
  // Make sure that the address is sector-aligned
  assert((addr & 0x1FF) == 0);

  spi_set_msb(ADESTO_FLASH_BUS);

  flash_write_buffer(data, size);
  flash_commit_buffer(addr);
  busy_wait_for_ready();
}

void config_flash(void)
{

  pin_config(ADESTO_FLASH_RESET_GPIO, ADESTO_FLASH_RESET, PINMODE_OUTPUT);
  pin_config(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS, PINMODE_OUTPUT);

  pin_set(ADESTO_FLASH_RESET_GPIO, ADESTO_FLASH_RESET);
  pin_set(ADESTO_FLASH_CS_GPIO, ADESTO_FLASH_CS);

  /* Reset the flash memory and wait until it's ready */
  flash_reset();
  busy_wait_for_ready();

  /**
   * XXX: Only uncomment below in testing ; this blows away the entire first
   * sector.
   *
   * assert(test_flash() == 0);
   */
  
  dbg_print("Finished configuring flash memory.\n");
}

int test_flash(void)
{
  int i = 0;

  flash_read(0, g.flash_buffer.data, STORAGE_PAGE_SIZE);

  for (i = 0; i < STORAGE_PAGE_SIZE; i++) {
    g.flash_buffer.data[i] = i % 256;
  }

  flash_write(0, g.flash_buffer.data, STORAGE_PAGE_SIZE);

  memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);

  flash_read(0, g.flash_buffer.data, STORAGE_PAGE_SIZE);

  for (i = 0; i < STORAGE_PAGE_SIZE; i++) {
    if (g.flash_buffer.data[i] != i % 256) {
      return 1;
    }
  }
  return 0;
}


