
#ifndef __STATE_H
#define __STATE_H

/**
 * Possible states that we can be in. This is also used for tagging alarms
 */
enum fall_state_t {
  F_STATE_SLEEP = 1,
  F_STATE_AWAKE,
  // The next three states can be used to tag an alarm
  F_STATE_AIRPLANE,
  F_STATE_FREEFALL,
  F_STATE_CANOPY
};


#endif
