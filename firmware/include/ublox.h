
#ifndef __UBLOX_H
#define __UBLOX_H

//! Header prepended to ubx binary messages
#define HDR_CHKSM_LENGTH 8 //(includes "sync1 sync2 classid msgid length checksum")
#define UBX_SYNC_BYTE_1 0xB5
#define UBX_SYNC_BYTE_2 0x62
    
//! UBX Protocol Class/Message ID's
#define MSG_CLASS_ACK 0X05
    #define MSG_ID_ACK_ACK 0x01
    #define MSG_ID_ACK_NAK 0x00
#define MSG_CLASS_AID 0x0B
    #define MSG_ID_AID_ALM 0x30
    #define MSG_ID_AID_ALPSRV 0X32
    #define MSG_ID_AID_ALP 0x50
    #define MSG_ID_AID_AOP 0x33
    #define MSG_ID_AID_DATA 0x10
    #define MSG_ID_AID_EPH 0x31
    #define MSG_ID_AID_HUI 0x02
    #define MSG_ID_AID_INI 0x01
    #define MSG_ID_AID_REQ 0x00
#define MSG_CLASS_CFG 0x06
    #define MSG_ID_CFG_ANT 0X13
    #define MSG_ID_CFG_CNFGR 0x09
    #define MSG_ID_CFG_DAT 0x06
    #define MSG_ID_CFG_EKF 0x12
    #define MSG_ID_CFG_ESFGWT 0x29
    #define MSG_ID_CFG_FXN 0x0E
    #define MSG_ID_CFG_ITFM 0x39
    #define MSG_ID_CFG_MSG 0x01
    #define MSG_ID_CFG_NAV5 0x24
    #define MSG_ID_CFG_NAVX5 0x23
    #define MSG_ID_CFG_NMEA 0x17
    #define MSG_ID_CFG_NVS 0x22
    #define MSG_ID_CFG_PM2 0x3B
    #define MSG_ID_CFG_PM 0x32
    #define MSG_ID_CFG_PRT 0x00
    #define MSG_ID_CFG_RATE 0x08
    #define MSG_ID_CFG_RINV 0x34
    #define MSG_ID_CFG_RST 0x04
    #define MSG_ID_CFG_RXM 0x11
    #define MSG_ID_CFG_SBAS 0x16
    #define MSG_ID_CFG_TMODE2 0x3D
    #define MSG_ID_CFG_TMODE 0x1D
    #define MSG_ID_CFG_TP5 0x31
    #define MSG_ID_CFG_TP 0x07
    #define MSG_ID_CFG_USB 0x1B
#define MSG_CLASS_ESF 0x10
    #define MSG_ID_ESF_MEAS 0x02
    #define MSG_ID_ESF_STATUS 0x10
#define MSG_CLASS_INF 0x04
    #define MSG_ID_INF_DEBUG 0x04
    #define MSG_ID_INF_ERROR 0x00
    #define MSG_ID_INF_NOTICE 0x02
    #define MSG_ID_INF_TEST 0x03
    #define MSG_ID_INF_WARNING 0x01
#define MSG_CLASS_MON 0x0A
    #define MSG_ID_MON_HW2 0x0B
    #define MSG_ID_MON_HW 0x09
    #define MSG_ID_MON_IO 0x02
    #define MSG_ID_MON_MSGPP 0x06
    #define MSG_ID_MON_RXBUF 0x07
    #define MSG_ID_MON_RXR 0x21
    #define MSG_ID_MON_TXBUF 0X08
    #define MSG_ID_MON_VER 0x04
#define MSG_CLASS_NAV 0x01
    #define MSG_ID_NAV_AOPSTATUS 0x60
    #define MSG_ID_NAV_CLOCK 0x22
    #define MSG_ID_NAV_DGPS 0x31
    #define MSG_ID_NAV_DOP 0x04
    #define MSG_ID_NAV_EKFSTATUS 0x40
    #define MSG_ID_NAV_POSECEF 0x01
    #define MSG_ID_NAV_POSLLH 0x02
    #define MSG_ID_NAV_SBAS 0x32
    #define MSG_ID_NAV_SOL 0x06
    #define MSG_ID_NAV_PVT 0x07
    #define MSG_ID_NAV_STATUS 0x03
    #define MSG_ID_NAV_SVINFO 0x30
    #define MSG_ID_NAV_TIMEGPS 0x20
    #define MSG_ID_NAV_TIMEUTC 0x21
    #define MSG_ID_NAV_VELECEF 0x11
    #define MSG_ID_NAV_VELNED 0x12
#define MSG_CLASS_RXM 0x02
    #define MSG_ID_RXM_ALM 0x30
    #define MSG_ID_RXM_EPH 0x31
    #define MSG_ID_RXM_PMREQ 0x41
    #define MSG_ID_RXM_RAW 0x10
    #define MSG_ID_RXM_SFRB 0x11
    #define MSG_ID_RXM_SVSI 0x20
#define MSG_CLASS_TIM 0x0D
    #define MSG_ID_TIM_SVIN 0x04
    #define MSG_ID_TIM_TM2 0x03
    #define MSG_ID_TIM_TP 0x01
    #define MSG_ID_TIM_VRFY 0x06


struct ubx_header_s {
  uint8_t sync1, sync2;
  uint8_t msg_class;
  uint8_t msg_id;
  uint16_t length;
} __attribute__((packed));

struct ubx_nav_status_s {
  /* GPS time of week of the navigation epoch */
  uint32_t iTOW;
  /* ublox_fixtype_e */
  uint8_t gpsFix; 
  /* navigation status flags (ublox_fixtype_e) */
  uint8_t flags; 
  uint8_t fixStat;
  uint8_t flags2; 
  uint32_t ttff;
  uint32_t msss;
} __attribute__((packed));

/*    */
struct ubx_nav_pvt_solution_s {
  uint32_t iTOW;
  uint16_t year;
  uint8_t month, day, hour, min, sec;
  uint8_t valid;
  uint32_t tAcc;
  int32_t nano;
  uint8_t fixType;
  uint8_t flags;
  uint8_t _reserved1;
  uint8_t numSV;
  int32_t lon, lat;
  int32_t height;
  int32_t hMSL;
  uint32_t hAcc, vAcc;
  int32_t velN, velE, velD;
  int32_t gSpeed;
  int32_t heading;
  uint32_t sAcc;
  uint32_t headingAcc;
  uint16_t pDOP;
  int16_t _reserved2;
  uint32_t _reserved3;
} __attribute__((packed));

/* */
struct ubx_nav_solution_s {
  /*    */
  uint32_t iTOW;
  /*    */
  uint32_t fTOW;
  /*    */
  uint16_t week;
  /*    */
  uint8_t gpsFix;
  /*    */
  uint8_t flags;
  /*    */
  int32_t ecefX, ecefY, ecefZ;
  /*    */
  uint32_t pAcc;
  /*    */
  int32_t ecefVX, ecefVY, ecefVZ;
  /*    */
  uint32_t sAcc;
  /*    */
  uint16_t pDOP;
  /*    */
  uint8_t _reserved1;
  /*    */
  uint8_t numSV;
  /*    */
  uint32_t _reserved2;
} __attribute__((packed));

enum ublox_fixtype_e {
  FIXTYPE_NO_FIX = 0x00,
  FIXTYPE_DEAD_RECKONING,
  FIXTYPE_2D_FIX,
  FIXTYPE_3D_FIX,
  FIXTYPE_GPS_DEAD_RECKONING,
  FIXTYPE_TIME_ONLY
};

enum ublox_nav_flags_e {
  NAV_FLAGS_FIX_OK = 0x01,
  NAV_FLAGS_DIFF_SOLN = 0x02,
  NAV_FLAGS_WKNSET = 0x04,
  NAV_FLAGS_TOWSET = 0x08
};

enum ublox_nav_fixstat_e {
  NAV_FIXSTAT_DPGS = 0x01,
  NAV_FIXSTAT_MAP_MATCHING = 0xA0
};

/**
 * @brief Try to disocver the ublox module.
 *
 * @return 1 on success, 0 if unable to ping one
 */
int ublox_init(void);

/**
 * @brief Receiving a navigation solution (blocking)
 */
int ublox_get(void);

/**
 * @brief Ask module to start sending navigational messages.
 * 
 * @return 1 on success, 0 on failure.
 */
int ublox_start_updates(int rate);

/**
 * @brief Set the measuring rate of measurements.
 *
 * @brief ms The rate of measurement, in milliseconds
 */
int ublox_set_measuring_rate(uint16_t ms);
#endif // __UBLOX_H
