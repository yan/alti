
#ifndef __ALARM_H
#define __ALARM_H


#define ALARM_LEN         16

struct alarm_s {
  uint32_t state : 3;
  uint32_t mbarc : 29;
};



#endif // __ALARM_H
