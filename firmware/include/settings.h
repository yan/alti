#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <stddef.h>
#include <util.h>
#include <logger.h>

/**
 * Settings module is responsbile for configuring alarms and other settings
 * via the BLE link
 */

/**
 * Various options we can configure
 */
enum setting_e {
  SETTING_FREEFALL_ALARM1,
  SETTING_FREEFALL_ALARM2,
  SETTING_FREEFALL_ALARM3,
  SETTING_GEOFENCE1,
  SETTING_GEOFENCE2,
  SETTING_HOME_DZ,
  SETTING_DEVICENAME,
};

enum config_msg_e {
  CONFIG_SETTING,
  CONFIG_START_LOGGING,
  CONFIG_STOP_LOGGING,
  CONFIG_SET_EVENT,
  CONFIG_GET_EVENTDATA,
  CONFIG_LIST_EVENTS,
  CONFIG_FORMAT_STORAGE,
  CONFIG_RESPONSE_OK,
  CONFIG_RESPONSE_FAIL,

  SETTING_LAST
};

/**
 * This must be 18 bytes long
 */
struct setting_value_s {
  uint16_t type; // setting_e

  /* All values should pad out to 16 bytes */
#define SETTING_VAL_SIZE 16
  union {
    uint16_t vals16  [SETTING_VAL_SIZE / sizeof(uint16_t)];
    uint32_t val32   [SETTING_VAL_SIZE / sizeof(uint32_t)];
    char     str     [SETTING_VAL_SIZE];
    float    floatp  [SETTING_VAL_SIZE / sizeof(float)];
    double   doublep [SETTING_VAL_SIZE / sizeof(double)];
  };
#undef SETTING_VAL_SIZE
} __attribute__((packed));

/* Each setting must be at most 18 bytes */

/**
 *
 */
struct setting_event_s {
  /**
   * The id we're looking for. 0 for 'most recent'
   */
  uint32_t event_id;

  /**
   * Return code for the response
   */
  uint32_t status;

} __attribute__((packed));

/**
 *
 */
struct setting_response_s {
  uint32_t status;
} __attribute__((packed));

/**
 * TODO: Confirm that 20 bytes is what we have to work with for a response
 *
 */
struct setting_packet_s {
  uint8_t type;
  uint8_t crc7; // Unused for now

  /* This union must be at most 18 bytes */
  union {
    struct setting_value_s setting;
    struct setting_event_s event;
    struct setting_response_s response;
    struct event_header_s  event_data;
    uint8_t _settings_pad[18];
  };
} __attribute__((packed));

STRUCT_SIZE_ASSERT(struct setting_packet_s, 20);

/**
 *
 */
int settings_apply(struct setting_packet_s *packet);

#endif

