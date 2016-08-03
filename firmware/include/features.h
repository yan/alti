
#ifndef __FEATURES_H
#define __FEATURES_H


/* Enable GPS support */
#define CONFIG_USE_GPS            ( 1 )

/* Enable accelerometer support */
#define CONFIG_USE_ACCEL          ( 1 )

/* Enable gyroscope support */
#define CONFIG_USE_GYRO           ( 1 )

/* Enable magnetometer support */
#define CONFIG_USE_MAG            ( 0 )

/** Do not set this to zero */
#define CONFIG_USE_USART_ISR      ( 1 )

/** Debugging counters that should generally be disabled */
#define CONFIG_USE_COUNTERS       ( 1 )

/* Low-pass filter sensor values */
#define CONFIG_ENABLE_LP_FILTER   ( 0 )

/* Speed to run the ublox gps module at */
#define CONFIG_UBLOX_BAUD_RATE    ( 230400 )

/* Add debugging checks for handling flash */
#define ENABLE_FLASH_DEBUG        ( 1 )





/* ------- DO NOT EDIT BELOW THIS LINE ------------------------------------- */

/* If we aren't using the GPS, no need to include all the usart isr code */
#if (CONFIG_USE_GPS == 0)
#  undef CONFIG_USE_USART_ISR
#  define CONFIG_USE_USART_ISR      ( 0 )
#endif

#define CONFIG_FEATURES          (    0 \
    /* Baro is always enbled */       | \
    1                          << 0   | \
    CONFIG_USE_ACCEL           << 1   | \
    CONFIG_USE_GYRO            << 2   | \
    CONFIG_USE_MAG             << 3   | \
    CONFIG_USE_GPS             << 4   | \
    0 )




#endif // __FEATURES_H
