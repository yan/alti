
#ifndef __CONFIG_H
#define __CONFIG_H

#define BLE_EXTI_ISR_PRIORITY     (0x1)

#define MAIN_EVENT_LOOP_TIMEOUT   (1000 / portTICK_PERIOD_MS)

void config_clock(void);

#endif
