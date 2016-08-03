#ifndef ALTISENSORPACKET_H
#define ALTISENSORPACKET_H

#include <QString>
#include <QDataStream>
#include <QGeoCoordinate>
#include <sample.h>
#include <settings.h>
// #include "altisensoraccel.h"
// #include "altisensorgps.h"

template <typename T>
QDataStream &operator>>(QDataStream &s, T& dest);

template <> QDataStream &operator>>(QDataStream &s, sensor_packet_s& dest);
template <> QDataStream &operator>>(QDataStream &s, gps_sample_s& dest);
template <> QDataStream &operator>>(QDataStream &s, accel_sample_s& dest);
template <> QDataStream &operator>>(QDataStream &s, config_packet_s& dest);

template <typename T> QString to_s(const T &src);

template <> QString to_s(const sensor_packet_s &src);
template <> QString to_s(const gps_sample_s &src);
template <> QString to_s(const accel_sample_s &src);

class AltiSensorAccel {
    Q_GADGET

    Q_PROPERTY(qint16 x MEMBER m_x)
    Q_PROPERTY(qint16 y MEMBER m_y)
    Q_PROPERTY(qint16 z MEMBER m_z)

public:
    AltiSensorAccel() :m_x(0), m_y(0), m_z(0) { }
    AltiSensorAccel(const accel_sample_s &sample)
        : m_x(sample.accel[0])
        , m_y(sample.accel[1])
        , m_z(sample.accel[2])
    {
    }

    bool operator!=(const AltiSensorAccel &rhs) const {
        return !(m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z);
    }

    Q_INVOKABLE QString toString() const {
        return QString("accel {x: %1, y: %2, z: %3}")
                .arg(m_x)
                .arg(m_y)
                .arg(m_z);

    }
private:
    qint16 m_x, m_y, m_z;
};
Q_DECLARE_METATYPE(AltiSensorAccel)

class AltiSensorGps {
    Q_GADGET

    Q_PROPERTY(double latitude MEMBER m_lat)
    Q_PROPERTY(double longitude MEMBER m_lon)
    Q_PROPERTY(qint32 accuracy MEMBER m_accuracy)
    Q_PROPERTY(qint32 speed MEMBER m_groundspeed)
    Q_PROPERTY(qint32 heading MEMBER m_heading)
    Q_PROPERTY(QGeoCoordinate coord READ asCoordinate)

public:
    AltiSensorGps()
        : m_lat(0.0) , m_lon(0.0) , m_accuracy(0) , m_groundspeed(0) , m_heading(0) { }

    AltiSensorGps(const gps_sample_s &sample)
        : m_lat(sample.lat / static_cast<double>(100.0))
        , m_lon(sample.lon / static_cast<double>(100.0))
        , m_accuracy(sample.accuracy)
        , m_groundspeed(sample.ground_speed)
        , m_heading(sample.heading)
    {
    }

    QGeoCoordinate asCoordinate() const {
        return QGeoCoordinate(m_lat, m_lon);
    }

    bool operator!=(const AltiSensorGps &rhs) const {
        return !(m_lat == rhs.m_lat &&
                 m_lon == rhs.m_lon &&
                 m_accuracy == rhs.m_accuracy &&
                 m_groundspeed == rhs.m_groundspeed &&
                 m_heading == rhs.m_heading);
    }

    Q_INVOKABLE QString toString() const {
        return QString("gps { lat: %1, lon: %2, accuracy: %3, speed: %4, heading: %5 }")
                .arg(m_lat)
                .arg(m_lon)
                .arg(m_accuracy)
                .arg(m_groundspeed)
                .arg(m_heading);
    }

private:
    double m_lat, m_lon;
    qint32 m_accuracy, m_groundspeed, m_heading;
};

Q_DECLARE_METATYPE(AltiSensorGps)


class AltiSensorPacket {
    Q_GADGET

    Q_PROPERTY(AltiSensorAccel accel MEMBER m_acc)
    Q_PROPERTY(AltiSensorGps gps MEMBER m_gps)
    Q_PROPERTY(qint32 mbarc MEMBER m_mbarc)
    Q_PROPERTY(qint32 ticks MEMBER m_ticks)

public:
    AltiSensorPacket():m_acc(), m_gps(), m_ticks(0), m_mbarc(0) {

    }

    AltiSensorPacket(const sensor_packet_s &packet )
        : m_acc(packet.accel_sample)
        , m_gps(packet.gps_sample)
        , m_ticks(packet.ticks)
        , m_mbarc(packet.mbarc)
    {
    }

    Q_INVOKABLE QString toString() const {
        return QString("sensor_packet_t { ticks: %1 mbarc: %2 %3 %4")
                .arg(m_ticks)
                .arg(m_mbarc)
                .arg(m_gps.toString())
                .arg(m_acc.toString());
    }

private:
    AltiSensorAccel m_acc;
    AltiSensorGps m_gps;
    qint32 m_ticks;
    qint32 m_mbarc;
};

Q_DECLARE_METATYPE(AltiSensorPacket)

#endif // ALTISENSORPACKET_H
