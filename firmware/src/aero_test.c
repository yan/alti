#if !TESTING
#  error "This file should only be built in test mode"
#endif

#include <string.h>
#include <aero.h>
#include <logger.h>
#include <flash.h>

int
aero_main(int argc, char *argv[])
{

  (void) argc;
  (void) argv;

  struct event_header_s event;
  struct sensor_packet_s packet;
  int i = 0;

  memset(&packet, '\0', sizeof packet); 

  logger_format_storage();
  logger_start_event(&event);
  for (i = 0; i < 256; i++){ 
    logger_write_sample(&event, &packet);
  }
  logger_end_event(&event);

  return 0;
}
