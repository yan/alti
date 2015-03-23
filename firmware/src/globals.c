#include <globals.h>

/** @brief FreeRTOSConfig.h uses this to refer to the clock rate */
uint32_t rcc_clock_freq;

/** @brief The main queue onto which events get pushed from ISRs and tasks */
void *main_queue_g;

/** @brief XXX */
void *status_queue_g;
