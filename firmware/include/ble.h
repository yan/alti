
#ifndef __BLE_H
#define __BLE_H

void config_ble(void);

#define PIPE_OPEN(p) \
   (g.pipes_open[(p)/8] & (1 << (p % 8)))

#define PIPE_CLOSED(p) \
   (g.pipes_closed[(p)/8] & (1 << (p % 8)))

#endif
