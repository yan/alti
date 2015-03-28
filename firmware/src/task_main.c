
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <config.h>
#include <nrf8001.h>
#include <ble.h>
#include <events.h>
#include <util.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_status_led.h>

#include <services.h> // delete me after debugging ble updates
#include <string.h> // delete me after debugging ble updates

int g_given = 0, g_events_received = 0, g_events_processed = 0;
int g_should_send = 0;

void task_main(void *p)
{
  // QueueHandle_t main_queue = (QueueHandle_t) p;
  (void) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  struct nrf8001_cmd_s cmd;
  enum global_state_e state = GLOBAL_STATE_RESET;
  unsigned int i = 0;

  memset(&cmd, '\0', sizeof(cmd));

  for (;;) {
    status = xQueueReceive(main_queue_g, &evt, MAIN_EVENT_LOOP_TIMEOUT);

    if (status == pdFAIL) {
      if (g_should_send) {
        cmd.opcode = 0x15;
        cmd.length = 5;
        cmd.data[0] = PIPE_AERO_PRESSURE_BAROMETRIC_PRESSURE_TX;
        
        //int x = swap_endian(i);


        cmd.data[1] = i;// swap_endian(i);

        printf("sending: %x\n", i);
        i++;

        ble_send_cmd(&cmd);
      }

      continue;
    }

    g_events_received++;

    switch (evt.type) {
      case GLOBAL_EVT_RESET:
      state = GLOBAL_STATE_RESET;
      break;

      case GLOBAL_EVT_RCVD_I2C:
      break;

      case GLOBAL_EVT_RCVD_SPI:
      break;

      case GLOBAL_EVT_NRF8001_RDY: {
        xSemaphoreGive(ble_data_g->semphr);
        g_given++;
      }
      break;

      case GLOBAL_EVT_NRF8001_EVENT: {
        //struct nrf8001_cmd_s *evt = evt.payload;
        nrf8001_handle_event(evt.payload);
      }
      break;

      case GLOBAL_EVT_LAST:
      break;

      default:
      state = GLOBAL_STATE_RESET;
      break;
    }

    g_events_processed++;
  }

  (void) state; // XXX Remove me once we start using this

}
