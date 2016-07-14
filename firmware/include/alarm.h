
#ifndef __ALARM_H
#define __ALARM_H


#define ALARM_LEN         4

enum alarm_state_e {
  ALARM_STATE_ASCEND,
  ALARM_STATE_FREEFALL,
  ALARM_STATE_CANOPY,

  ALARM_STATE_LAST = 7
};

struct alarm_s {
  uint32_t state : 3;
  uint32_t mbarc : 29;
};



#endif // __ALARM_H
