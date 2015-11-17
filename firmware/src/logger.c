
#include <FreeRTOS.h>
#include <semphr.h>

#include <string.h>

#include <config.h>
#include <logger.h>
#include <sample.h>
#include <flash.h>
#include <globals.h>

void write_sensor_packet(struct sensor_packet_s *packet)
{
  // TODO: Revisit adding a real timeout here
  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);

  unsigned int remaining = WRITE_BUFFER_LEN - g.flash_buffer.write_offset;

  if (remaining < sizeof(*packet)) {
    memset(&g.flash_buffer.data[g.flash_buffer.write_offset], '\0', remaining);
    flash_write(0, g.flash_buffer.data, WRITE_BUFFER_LEN);
    // memset(g.flash_buffer.data, '\0', WRITE_BUFFER_LEN);
    g.flash_buffer.write_offset = 0;
    dbg_print("Writing to flash");
  } 
  
  memcpy(&g.flash_buffer.data[g.flash_buffer.write_offset], packet, sizeof(*packet));
  g.flash_buffer.write_offset += sizeof(*packet);
  
  xSemaphoreGive(g.flash_buffer.lock);
}
