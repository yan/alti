#include "altisensorpacket.h"
#include <QDebug>

template <>
QDataStream &operator>>(QDataStream &s, gps_sample_s& dest)
{
    QDataStream::ByteOrder order = s.byteOrder();
    s.setByteOrder(QDataStream::LittleEndian);

    s >> dest.accuracy
      >> dest.lat
      >> dest.lon
      >> dest.ground_speed
      >> dest.heading;

    s.setByteOrder(order);
    return s;
}

template <>
QDataStream &operator>>(QDataStream &s, accel_sample_s& dest)
{
    QDataStream::ByteOrder order = s.byteOrder();
    s.setByteOrder(QDataStream::LittleEndian);

    s >> dest.accel[0]
      >> dest.accel[1]
      >> dest.accel[2];

    s.setByteOrder(order);
    return s;
}

template <>
QDataStream &operator>>(QDataStream& s, sensor_packet_s& dest)
{
    auto order = s.byteOrder();
    s.setByteOrder(QDataStream::LittleEndian);

    s >> dest.ticks;
    s >> dest.mbarc;
    s >> dest.gps_sample;
    s >> dest.accel_sample;

    s.setByteOrder(order);
    return s;
}

template <>
QDataStream &operator>>(QDataStream &s, config_packet_s& dest)
{
    auto order = s.byteOrder();
    s.setByteOrder(QDataStream::LittleEndian);
    s >> dest.type;
    switch (dest.type) {
    case CONFIG_SET_EVENT:
        s >> dest.event.event_id;
        s >> dest.event.status;
        break;
    default:
        break;
    }
    s.setByteOrder(order);
    return s;
}

template <>
QString to_s(const sensor_packet_s &src)
{
    return AltiSensorPacket(src).toString();
}

template <>
QString to_s(const gps_sample_s &src)
{
    return AltiSensorGps(src).toString();
}

template <>
QString to_s(const accel_sample_s &src)
{
    return AltiSensorAccel(src).toString();
}

