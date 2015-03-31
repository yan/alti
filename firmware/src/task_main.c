
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <config.h>
#include <globals.h>
#include <nrf8001.h>
#include <ble.h>
#include <events.h>
#include <util.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_alert.h>

#include <services.h> // delete me after debugging ble updates
#include <aci_cmds.h> // delete me after debugging ble updates
#include <string.h> // delete me after debugging ble updates

int g_given = 0, g_events_received = 0, g_events_processed = 0;

void task_main(void *p)
{
  (void) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  struct nrf8001_cmd_s cmd;
  enum global_state_e state = GLOBAL_STATE_RESET;

  memset(&cmd, '\0', sizeof(cmd));

  for (;;) {
    status = xQueueReceive(g.main_queue_g, &evt, MAIN_EVENT_LOOP_TIMEOUT);

    if (status == pdFAIL) {
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

      case GLOBAL_EVT_NRF8001_PIPES_CHANGED: {
        unsigned int new_rate;
        if (PIPE_OPEN(PIPE_AERO_PRESSURE_BAROMETRIC_PRESSURE_TX)) {
          new_rate = 100 / portTICK_PERIOD_MS;
          dbg_print("Starting to send pressure\n");
        } else {
          new_rate = portMAX_DELAY;
          dbg_print("Stopping to send pressure\n");
        }
          
        xQueueSend(g.baro_queue_g, &new_rate, 0);
      }
        break;

      case GLOBAL_EVT_AIR_PRESSURE: {
        unsigned int pressure = (unsigned int) evt.payload;
        const uint8_t tx_pipe = PIPE_AERO_PRESSURE_BAROMETRIC_PRESSURE_TX;

        if (PIPE_OPEN(tx_pipe)) {
          ble_tx(tx_pipe, (void*)&pressure, sizeof(pressure));
        }
      }
        break;

      case GLOBAL_EVT_NRF8001_RDY: {
        xSemaphoreGive(g.ble_data_g->semphr);
        g_given++;
      }
        break;

      case GLOBAL_EVT_NRF8001_EVENT: 
        nrf8001_handle_event(evt.payload);
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
