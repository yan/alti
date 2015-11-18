

#include <FreeRTOS.h>
#include <queue.h>

#include <config.h>

#if CONFIG_USE_GPS

#include <semphr.h>

#include <events.h>
#include <globals.h>
#include <task_gps.h>
#include <util.h>
#include <logger.h>
#include <hal.h>
#include <pins.h>

#include <ublox.h>
BaseType_t usart_given = 0;
/**
 *
 */
void config_gps(void)
{
  ublox_init();
  ublox_start_updates(1);
}

extern void usart1_isr(void);
void usart1_isr(void)
{
  static BaseType_t higher_priority;

  usart_disable_rx_interrupt(UBLOX_UART);

  usart_given++;

  xSemaphoreGiveFromISR(g.usart_mutex_g, &higher_priority);

  portYIELD_FROM_ISR(higher_priority);
}

/**
 *
 */
void task_gps(void *p)
{
  (void) p;

  enum {
    SLEEP,
    RUN
  } state = RUN;

  BaseType_t received;
  BaseType_t sleep_period = MS_TO_TICKS(100);
  BaseType_t status;

  config_gps();
  xSemaphoreGive(g.usart_mutex_g);

  for (;;) {
    status = xQueueReceive(g.gps_queue_g, &received, sleep_period);

    if (status != pdPASS) {
      while (state == RUN) {

        xSemaphoreTake(g.usart_mutex_g, portMAX_DELAY);
        ublox_get();
        arch_enable_usart_interrupt(UBLOX_UART);

        // send to main queue ??
        //
      }
      continue; // ??
    }

    switch (received) {
      case EVT_GPS_START:
        ublox_start_updates(1);
        state = RUN;
        break;

      case EVT_GPS_SLEEP:
        ublox_sleep();
        state = SLEEP;
        break;

      case EVT_GPS_UPDATE_RTC:
        // do that
        break;

      default:
        assert(0);
        break;
    }
  }
}
#endif // CONFIG_USE_GPS
