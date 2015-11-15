#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/* Here is a good place to include header files that are required across
your application. */

#include <globals.h>

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configUSE_TICKLESS_IDLE                 0
#define configCPU_CLOCK_HZ                      ( g.rcc_clock_freq )
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    5
#define configMINIMAL_STACK_SIZE                128
#define configTOTAL_HEAP_SIZE                   10240
#define configMAX_TASK_NAME_LEN                 8
#define configUSE_16_BIT_TICKS                  0
#define configIDLE_SHOULD_YIELD                 1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configUSE_ALTERNATIVE_API               0 /* Deprecated! */
/** TODO: Comment this out later */
#define configQUEUE_REGISTRY_SIZE               20
#define configUSE_QUEUE_SETS                    0
#define configUSE_TIME_SLICING                  0
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0

/* Hook function related definitions. */
#define configUSE_IDLE_HOOK                     0
#define configUSE_TICK_HOOK                     0
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            1

/* Run time and task stats gathering related definitions. */
#define configGENERATE_RUN_TIME_STATS           0
#define configUSE_TRACE_FACILITY                0
#define configUSE_STATS_FORMATTING_FUNCTIONS    0

/* Co-routine related definitions. */
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         1

/* Software timer related definitions. */
#define configUSE_TIMERS                        1
#define configTIMER_TASK_PRIORITY               3
#define configTIMER_QUEUE_LENGTH                10
#define configTIMER_TASK_STACK_DEPTH            configMINIMAL_STACK_SIZE

/* Interrupt nesting behaviour configuration. */

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
        /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
        #define configPRIO_BITS                 __NVIC_PRIO_BITS
#else
        #define configPRIO_BITS                 4        /* 15 priority levels */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
// #define configMAX_API_CALL_INTERRUPT_PRIORITY   [dependent on processor and application]

//#define vAssertCalled(f, l)  __asm("BKPT 0")

/* Define to trap errors during development. */
//__asm("BKPT 0")
#define configASSERT( x )     if( ( x ) == 0 ) for (;;)

/* FreeRTOS MPU specific definitions. */
#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0

/* Optional functions - most linkers will remove unused functions anyway. */
#define INCLUDE_vTaskPrioritySet                0
#define INCLUDE_uxTaskPriorityGet               0
#define INCLUDE_vTaskDelete                     0
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xResumeFromISR                  1
#define INCLUDE_vTaskDelayUntil                 1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetSchedulerState          0
#define INCLUDE_xTaskGetCurrentTaskHandle       0
#define INCLUDE_uxTaskGetStackHighWaterMark     0
#define INCLUDE_xTaskGetIdleTaskHandle          0
#define INCLUDE_xTimerGetTimerDaemonTaskHandle  0
#define INCLUDE_pcTaskGetTaskName               0
#define INCLUDE_eTaskGetState                   0
#define INCLUDE_xEventGroupSetBitFromISR        0
#define INCLUDE_xTimerPendFunctionCall          0

/* aero-specific config */
#define DEFAULT_TASK_PRIORITY                  ( tskIDLE_PRIORITY + 1 )
/* A header file that defines trace macro can be included here. */

#define vPortSVCHandler                         sv_call_handler
#define xPortPendSVHandler                      pend_sv_handler
#define xPortSysTickHandler                     sys_tick_handler


#define CONFIG_TASK_MAIN_STACK_DEPTH            768 // 3k
#define CONFIG_TASK_MAIN_QUEUE_LEN              32
#define CONFIG_TASK_MAIN_PRIORITY               ( DEFAULT_TASK_PRIORITY )

#define CONFIG_TASK_ALERT_QUEUE_LEN             4
#define CONFIG_TASK_ALERT_STACK_DEPTH           configMINIMAL_STACK_SIZE
#define CONFIG_TASK_ALERT_PRIORITY              ( DEFAULT_TASK_PRIORITY+1 )

#define CONFIG_TASK_BLE_QUEUE_LEN               4
#define CONFIG_TASK_BLE_STACK_DEPTH             256
#define CONFIG_TASK_BLE_PRIORITY                ( DEFAULT_TASK_PRIORITY )

#define CONFIG_TASK_SENSOR_QUEUE_LEN               4
#define CONFIG_TASK_SENSOR_STACK_DEPTH             256
#define CONFIG_TASK_SENSOR_PRIORITY                ( DEFAULT_TASK_PRIORITY+1 )

#if defined(ENABLE_SEMIHOSTING)
#  undef configTOTAL_HEAP_SIZE
#  define configTOTAL_HEAP_SIZE                  (49152 - 36444)

#  undef CONFIG_TASK_MAIN_STACK_DEPTH
#  define CONFIG_TASK_MAIN_STACK_DEPTH           1024

#  undef CONFIG_TASK_ALERT_STACK_DEPTH
#  define CONFIG_TASK_ALERT_STACK_DEPTH          256

#  undef  CONFIG_TASK_BLE_STACK_DEPTH
#  define CONFIG_TASK_BLE_STACK_DEPTH            512

#  undef CONFIG_TASK_SENSOR_STACK_DEPTH
#  define CONFIG_TASK_SENSOR_STACK_DEPTH         600
#endif

#endif /* FREERTOS_CONFIG_H */

