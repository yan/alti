#include <settings.h>
#include <globals.h>
#include <util.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int settings_apply(struct setting_value_s *value)
{
  assert(value != NULL);

  switch (value->type) {
  case SETTING_FREEFALL_ALARM1:
  case SETTING_FREEFALL_ALARM2:
  case SETTING_FREEFALL_ALARM3: {
    /* TODO: What are valid values? */
    if (value->val32[0] < 10000 || value->val32[0] > 200000) {
      return 0;
    }
    g.persisted_config.alarms[value->type - SETTING_FREEFALL_ALARM1].mbarc =
      value->val32[0];
  }
  break;

  case SETTING_GEOFENCE1:
  case SETTING_GEOFENCE2:
    g.persisted_config.geofences[value->type - SETTING_GEOFENCE1].lat =
      (int32_t) value->val32[0];
    g.persisted_config.geofences[value->type - SETTING_GEOFENCE1].lon =
      (int32_t) value->val32[1];
    break;

  case SETTING_HOME_DZ_ONE:
  case SETTING_HOME_DZ_TWO:
  {
    int i = 0;
    size_t offset = (value->type - SETTING_HOME_DZ_ONE) * SETTING_VAL_SIZE;

    for (i = 0; i < SETTING_VAL_SIZE; i++) {
      if (offset + i >= HOME_DZ_LEN) {
        return 0;
      }

      g.persisted_config.home_dz[offset + i] = value->str[i];
    }
  } 
  break;

  case SETTING_DEVICENAME:
  break;

  default:
    return 0;
  }

  return 1;
}

#ifdef __cplusplus
}
#endif // __cplusplus
