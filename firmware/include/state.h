
#ifndef __STATE_H
#define __STATE_H

#if 0
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

/**
 *
 *
 */
typedef enum global_state_e {
  GLOBAL_STATE_RESET,
  GLOBAL_STATE_BLE_SETUP,
  GLOBAL_STATE_SLEEP,
  GLOBAL_STATE_PERIODIC,
  GLOBAL_STATE_ACTIVE,
  GLOBAL_STATE_TRANSFERRING_DATA
} global_state_t;

#endif
