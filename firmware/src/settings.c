#include <settings.h>
#include <util.h>


int settings_apply(struct setting_packet_s *packet)
{
  assert(packet != NULL);
  int i = 0;

  switch (packet->type) {
  case SETTING_FREEFALL_ALARM1:
    break;

  case SETTING_FREEFALL_ALARM2:
    break;

  case SETTING_FREEFALL_ALARM3:
    break;

  case SETTING_GEOFENCE1:
    break;

  case SETTING_GEOFENCE2:
    break;

  case SETTING_HOME_DZ:
    break;

  case SETTING_DEVICENAME:
    break;
  }

  return i;
}

