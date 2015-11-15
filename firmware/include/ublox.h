
#ifndef __UBLOX_H
#define __UBLOX_H

/* To closer match the ublox pdf */
typedef int32_t I4;
typedef int16_t I2;
typedef int8_t I1;
typedef uint32_t U4;
typedef uint32_t X4;
typedef uint16_t U2;
typedef uint16_t X2;
typedef uint8_t X1;
typedef uint8_t U1;

//! Header prepended to ubx binary messages
#define HDR_CHKSM_LENGTH 8 //(includes "sync1 sync2 classid msgid length checksum")
#define UBX_SYNC_BYTE_1 0xB5
#define UBX_SYNC_BYTE_2 0x62
    
//! UBX Protocol Class/Message ID's
#define MSG_CLASS_ACK 0X05
    #define MSG_ID_ACK_NAK 0x00
    #define MSG_ID_ACK_ACK 0x01
#define MSG_CLASS_AID 0x0B
    #define MSG_ID_AID_REQ 0x00
    #define MSG_ID_AID_INI 0x01
    #define MSG_ID_AID_HUI 0x02
    #define MSG_ID_AID_DATA 0x10
    #define MSG_ID_AID_ALM 0x30
    #define MSG_ID_AID_EPH 0x31
    #define MSG_ID_AID_ALPSRV 0X32
    #define MSG_ID_AID_AOP 0x33
    #define MSG_ID_AID_ALP 0x50
#define MSG_CLASS_CFG 0x06
    #define MSG_ID_CFG_PRT 0x00
    #define MSG_ID_CFG_MSG 0x01
    #define MSG_ID_CFG_RST 0x04
    #define MSG_ID_CFG_DAT 0x06
    #define MSG_ID_CFG_TP 0x07
    #define MSG_ID_CFG_RATE 0x08
    #define MSG_ID_CFG_CNFGR 0x09
    #define MSG_ID_CFG_FXN 0x0E
    #define MSG_ID_CFG_RXM 0x11
    #define MSG_ID_CFG_EKF 0x12
    #define MSG_ID_CFG_ANT 0x13
    #define MSG_ID_CFG_SBAS 0x16
    #define MSG_ID_CFG_NMEA 0x17
    #define MSG_ID_CFG_USB 0x1B
    #define MSG_ID_CFG_TMODE 0x1D
    #define MSG_ID_CFG_NVS 0x22
    #define MSG_ID_CFG_NAVX5 0x23
    #define MSG_ID_CFG_NAV5 0x24
    #define MSG_ID_CFG_ESFGWT 0x29
    #define MSG_ID_CFG_TP5 0x31
    #define MSG_ID_CFG_PM 0x32
    #define MSG_ID_CFG_RINV 0x34
    #define MSG_ID_CFG_ITFM 0x39
    #define MSG_ID_CFG_PM2 0x3B
    #define MSG_ID_CFG_TMODE2 0x3D
#define MSG_CLASS_ESF 0x10
    #define MSG_ID_ESF_MEAS 0x02
    #define MSG_ID_ESF_STATUS 0x10
#define MSG_CLASS_INF 0x04
    #define MSG_ID_INF_ERROR 0x00
    #define MSG_ID_INF_WARNING 0x01
    #define MSG_ID_INF_NOTICE 0x02
    #define MSG_ID_INF_TEST 0x03
    #define MSG_ID_INF_DEBUG 0x04
#define MSG_CLASS_MON 0x0A
    #define MSG_ID_MON_IO 0x02
    #define MSG_ID_MON_VER 0x04
    #define MSG_ID_MON_MSGPP 0x06
    #define MSG_ID_MON_RXBUF 0x07
    #define MSG_ID_MON_TXBUF 0X08
    #define MSG_ID_MON_HW 0x09
    #define MSG_ID_MON_HW2 0x0B
    #define MSG_ID_MON_RXR 0x21
#define MSG_CLASS_NAV 0x01
    #define MSG_ID_NAV_POSECEF 0x01
    #define MSG_ID_NAV_POSLLH 0x02
    #define MSG_ID_NAV_STATUS 0x03
    #define MSG_ID_NAV_DOP 0x04
    #define MSG_ID_NAV_SOL 0x06
    #define MSG_ID_NAV_PVT 0x07
    #define MSG_ID_NAV_VELECEF 0x11
    #define MSG_ID_NAV_VELNED 0x12
    #define MSG_ID_NAV_TIMEGPS 0x20
    #define MSG_ID_NAV_TIMEUTC 0x21
    #define MSG_ID_NAV_CLOCK 0x22
    #define MSG_ID_NAV_SVINFO 0x30
    #define MSG_ID_NAV_DGPS 0x31
    #define MSG_ID_NAV_SBAS 0x32
    #define MSG_ID_NAV_EKFSTATUS 0x40
    #define MSG_ID_NAV_AOPSTATUS 0x60
#define MSG_CLASS_RXM 0x02
    #define MSG_ID_RXM_RAW 0x10
    #define MSG_ID_RXM_SFRB 0x11
    #define MSG_ID_RXM_SVSI 0x20
    #define MSG_ID_RXM_ALM 0x30
    #define MSG_ID_RXM_EPH 0x31
    #define MSG_ID_RXM_PMREQ 0x41
#define MSG_CLASS_TIM 0x0D
    #define MSG_ID_TIM_TP 0x01
    #define MSG_ID_TIM_TM2 0x03
    #define MSG_ID_TIM_SVIN 0x04
    #define MSG_ID_TIM_VRFY 0x06


struct ubx_header_s {
  uint8_t msg_class;
  uint8_t msg_id;
  uint16_t length;
} __attribute__((packed));

struct ubx_nav_status_s {
  /* GPS time of week of the navigation epoch */
  U4 iTOW;
  /* ublox_fixtype_e */
  U1 gpsFix; 
  /* navigation status flags (ublox_fixtype_e) */
  X1 flags; 
  X1 fixStat;
  X1 flags2; 
  U4 ttff;
  U4 msss;
} __attribute__((packed));

/*    */
struct ubx_nav_pvt_solution_s {
  U4 iTOW;
  U2 year;
  U1 month, day, hour, min, sec;
  X1 valid;
  U4 tAcc;
  I4 nano;
  U1 fixType;
  X1 flags;
  U1 _reserved1;
  U1 numSV;
  I4 lon, lat;
  I4 height;
  I4 hMSL;
  U4 hAcc, vAcc;
  I4 velN, velE, velD;
  I4 gSpeed;
  I4 heading;
  U4 sAcc;
  U4 headingAcc;
  U2 pDOP;
  X2 _reserved2;
  U4 _reserved3;
} __attribute__((packed));

/* */
struct ubx_nav_solution_s {
  U4 iTOW; /*    */
  I4 fTOW; /*    */
  I2 week; /*    */
  U1 gpsFix; /*    */
  X1 flags; /*    */
  I4 ecefX, ecefY, ecefZ; /*    */
  U4 pAcc; /*    */
  I4 ecefVX, ecefVY, ecefVZ; /*    */
  U4 sAcc; /*    */
  U2 pDOP; /*    */
  U1 _reserved1; /*    */
  U1 numSV; /*    */
  U4 _reserved2; /*    */
} __attribute__((packed));

struct ublox_port_config_s {
  U1 portID;
  U1 reserved0;
  X2 txReady;
  X4 mode;
  U4 baudRate;
  X2 inProtoMask;
  X2 outProtoMask;
  X2 flags;
  U2 reserved5;
} __attribute__((packed));

struct ublox_cfg_pm2_s {
  U1 version;
  U1 reserved[3];
  X4 flags;
  U4 updatePeriod;
  U4 searchPeriod;
  U4 gridOffset;
  U2 onTime;
  U2 minAcqTime;
  U2 reserved4[2];
  U4 reserved6[2];
  U1 reserved8[2];
  U2 reserved10;
  U4 reserved11;
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

enum dynamic_platform_model_e {
  DYN_MODEL_PORTABLE = 0,
  DYN_MODEL_STATIONARY = 2,
  DYN_MODEL_PEDESTRIAN = 3,
  DYN_MODEL_AUTOMOTIVE = 4,
  DYN_MODEL_SEA = 5,
  DYN_MODEL_AIRBORNE_1G = 6,
  DYN_MODEL_AIRBORNE_2G = 7,
  DYN_MODEL_AIRBORNE_4G = 8
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

int ublox_get_rate(void);

int ublox_hard_reset(void);
#endif // __UBLOX_H
