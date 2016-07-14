#include <globals.h>
#include <gtest/gtest.h>
#include <settings.h>

namespace {

class SettingsTest : public ::testing::Test {
  protected:
    SettingsTest() {
    }

    virtual ~SettingsTest() {
    }

    virtual void SetUp() {
      memset(&packet, '\0', sizeof(packet));
      memset(&g, '\0', sizeof(g));
    }

    virtual void TearDown() {
      memset(&g, '\xff', sizeof(g));
    }

    struct setting_value_s packet;
    int status;
};

TEST_F(SettingsTest, FailsWithBadType) {
  packet.type = SETTING_LAST;

  status = settings_apply(&packet);

  ASSERT_EQ(status, 0);
}

TEST_F(SettingsTest, FreeFallAlarm) {
  int i;
  const uint32_t kAltitude = 100800;
  int alarm_type = SETTING_FREEFALL_ALARM1;

  for (i = 0; alarm_type <= SETTING_FREEFALL_ALARM3; alarm_type++, i++) {

    packet.type = alarm_type;
    packet.val32[0] = kAltitude + i;

    status = settings_apply(&packet);
    
    ASSERT_EQ(g.persisted_config.alarms[i].mbarc, kAltitude + i);
    ASSERT_NE(status, 0);
  }
}

TEST_F(SettingsTest, FreeFallAlarmFailsForBadValue) {
  uint32_t kBadValues[] = {
    0, 0xffffffff, 50
  };

  for (auto val : kBadValues) {

    packet.type = SETTING_FREEFALL_ALARM1;
    packet.val32[0] = val;

    status = settings_apply(&packet);
    
    ASSERT_EQ(status, 0);
  }
}

TEST_F(SettingsTest, GeoFence) {
  packet.type = SETTING_GEOFENCE1;
  packet.val32[0] = 1;
  packet.val32[1] = 2;

  status = settings_apply(&packet);

  ASSERT_EQ(g.persisted_config.geofences[0].lat, packet.val32[0]);
  ASSERT_EQ(g.persisted_config.geofences[0].lon, packet.val32[1]);

}

}
