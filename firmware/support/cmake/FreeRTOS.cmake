

set (FREERTOS_HOME freertos/FreeRTOS/Source)

include_directories(${FREERTOS_HOME}/include)
include_directories(${FREERTOS_HOME}/portable/GCC/ARM_CM3)

set (FREERTOS_SRC
  "${FREERTOS_HOME}/croutine.c"
  "${FREERTOS_HOME}/event_groups.c"
  "${FREERTOS_HOME}/list.c"
  "${FREERTOS_HOME}/queue.c"
  "${FREERTOS_HOME}/tasks.c"
  "${FREERTOS_HOME}/timers.c"
  "${FREERTOS_HOME}/portable/GCC/ARM_CM3/port.c"
  "${FREERTOS_HOME}/portable/MemMang/heap_2.c")

add_library (freertos ${FREERTOS_SRC})
