#include <string.h>

#include <flash.h>
#include <pins.h>
#include <hal.h>
#include <globals.h>
#include <util.h>

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

  pin_set(FLASH_GPIO, FLASH_nRESET);
  pin_clear(FLASH_GPIO, FLASH_nRESET);
  for (i = 0; i < 30; i++)
    ;
  pin_set(FLASH_GPIO, FLASH_nRESET);
}

static void flash_write_buffer(uint8_t *data, size_t size)
{
  pin_clear(FLASH_GPIO, FLASH_nCS);

  /* First, write it to the flash buffer */
  arch_spi_xfer(FLASH_PORT, ADESTO_WRITE_BUFFER_2);

  /* Send 15 dummy bits, then 9 intra-sector offset. */
  arch_spi_xfer(FLASH_PORT, 0);
  arch_spi_xfer(FLASH_PORT, 0);
  arch_spi_xfer(FLASH_PORT, 0);

  /* Then, clock in the actual data */
  spi_send_buf(FLASH_PORT, data, size);

  /* And we're done */
  pin_set(FLASH_GPIO, FLASH_nCS);
}

static void flash_commit_buffer(uint32_t address)
{
  pin_clear(FLASH_GPIO, FLASH_nCS);

  arch_spi_xfer(FLASH_PORT, ADESTO_WRITE_BUFFER_2_TO_MEM_W_ER);
  arch_spi_xfer(FLASH_PORT, (address & 0xFF0000) >> 16);
  arch_spi_xfer(FLASH_PORT, (address & 0xFF00) >> 8);
  arch_spi_xfer(FLASH_PORT,  address & 0xFF);
  
  pin_set(FLASH_GPIO, FLASH_nCS);
}

static void flash_read_status(struct status_register_s *dest)
{
  spi_set_msb(FLASH_PORT);

  pin_clear(FLASH_GPIO, FLASH_nCS);

  uint16_t returned;

  arch_spi_xfer(FLASH_PORT, ADESTO_AUX_STATUS_REGISTER_READ);

  returned = spi_read_octets(FLASH_PORT, sizeof(*dest), BYTEORDER_MSB);

  returned = (returned & 0x00ff) << 8 | (returned & 0xff00) >> 8;

  * (uint16_t*) dest = returned;

  pin_set(FLASH_GPIO, FLASH_nCS);

}
void flash_read(uint32_t addr, uint8_t *data, size_t size)
{
  spi_set_msb(FLASH_PORT);

  pin_clear(FLASH_GPIO, FLASH_nCS);

  /* Read memory in low power mode */
  arch_spi_xfer(FLASH_PORT, ADESTO_READ_CONTINUOUS_ARR_READ_LP);

  /* Send the address */
  arch_spi_xfer(FLASH_PORT, (addr & 0xFF0000) >> 16);
  arch_spi_xfer(FLASH_PORT, (addr & 0xFF00) >> 8);
  arch_spi_xfer(FLASH_PORT,  addr & 0xFF);

  /* Then read as much as we asked for */
  spi_read_data(FLASH_PORT, data, size);

  pin_set(FLASH_GPIO, FLASH_nCS);
}

void flash_write(uint32_t addr, uint8_t *data, size_t size)
{
  // Make sure that the address is sector-aligned
  assert((addr & 0x1FF) == 0);

  spi_set_msb(FLASH_PORT);

  flash_write_buffer(data, size);
  flash_commit_buffer(addr);
  busy_wait_for_ready();
}

int test_flash(void)
{
  int i = 0;
  for (i = 0; i < 512; i++) {
    g.flash_buffer.data[i] = 3;// i % 256;
  }

  flash_write(512, g.flash_buffer.data, 512);

  memset(g.flash_buffer.data, '\0', 512);

  flash_read(0, g.flash_buffer.data, 512);

  for (i = 0; i < 512; i++) {
    if (g.flash_buffer.data[i] != i % 256) {
      return 1;
    }
  }
  return 0;
}

static void busy_wait_for_ready(void)
{
  struct status_register_s status_reg;

  status_reg.ready = 0;

  while (!status_reg.ready) {
    flash_read_status(&status_reg);
  }
}

void config_flash(void)
{

  pin_config(FLASH_GPIO, FLASH_nRESET, PINMODE_OUTPUT);
  pin_config(FLASH_GPIO, FLASH_nCS, PINMODE_OUTPUT);

  pin_set(FLASH_GPIO, FLASH_nRESET);
  pin_set(FLASH_GPIO, FLASH_nCS);

  /* Reset the flash memory and wait until it's ready */
  flash_reset();
  busy_wait_for_ready();

  assert(test_flash() == 0);
  
  dbg_print("Finished configuring flash memory.\n");
}
