
#ifndef __FEATURES_H
#define __FEATURES_H

#define CONFIG_USE_GPS            ( 1 )

#define CONFIG_USE_ACCEL          ( 1 )

#define CONFIG_USE_USART_ISR      ( 1 )






/* ------- DO NOT EDIT BELOW THIS LINE ------------------------------------- */

/* If we aren't using the GPS, no need to include all the usart isr code */
#if (CONFIG_USE_GPS == 0)
#  undef CONFIG_USE_USART_ISR
#  define CONFIG_USE_USART_ISR      ( 0 )
#endif

#endif // __FEATURES_H
