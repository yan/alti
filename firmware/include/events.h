

/**
 * @brief All possible events that can be pushed to the main "thread"
 */
typedef enum global_event_e {
  GLOBAL_EVT_RESET,

  GLOBAL_EVT_RCVD_I2C,

  GLOBAL_EVT_RCVD_SPI,

  GLOBAL_EVT_LAST
} global_event_t;
