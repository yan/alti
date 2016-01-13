#include <string.h>
#include <aero.h>
#include <logger.h>
#include <flash.h>

#include <gtest/gtest.h>

int
aero_main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);

  struct event_header_s event;
  struct sensor_packet_s packet;
  int i = 0;

  memset(&packet, '\0', sizeof packet); 

  logger_format_storage();

  event.event_id = 1;
  event.sample_size = sizeof(struct sensor_packet_s);
  event.features = 0xff;
  event.rtc_start=0xDD;

  logger_start_event(&event);
  for (i = 0; i < 1; i++){ 
    packet.ticks = i;
    logger_write_sample(&event, &packet);
    printf("======== %u\n", event.samples);
  }
  printf("ending event\n");
  logger_end_event(&event);

  return RUN_ALL_TESTS();
}
