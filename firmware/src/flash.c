#include <flash.h>
#include <pins.h>
#include <hal.h>
#include <globals.h>

inline static void reset_flash(void);

inline static void reset_flash(void)
{
  int i = 0;

  pin_set(FLASH_GPIO, FLASH_nRESET);
  pin_clear(FLASH_GPIO, FLASH_nRESET);
  for (i = 0; i < 30; i++)
    ;
  pin_set(FLASH_GPIO, FLASH_nRESET);
}

void config_flash(void)
{
  reset_flash();

}
