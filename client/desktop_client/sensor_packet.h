#ifndef SENSOR_PACKET_H
#define SENSOR_PACKET_H

#include <stdint.h>

#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QStringBuilder>
#include <QString>

struct sensor_packet_s {
    uint32_t ticks; // offset: 0
    uint32_t mbarc; // offset: 4
    struct gps_sample_s {
        int32_t accuracy;
        int32_t lat, lon;
        int32_t ground_speed;
        int32_t heading;
        QString toString() const {
            return QString("{lat: %1, lon: %2, ground speed: %3, heading: %4, accuracy: %5 }")
                        .arg(lat/1e7f).arg(lon/1e7f).arg(ground_speed).arg(heading).arg(accuracy / 100.0f);
        }
    } gps;

    struct accel_sample_s {
        int16_t x, y, z;
        QString toString() const {
            return QString("{x: %1, y %2, z: %3}").arg(x).arg(y).arg(z);
        }
    } acc;

    sensor_packet_s(QByteArray arr) {
        QDataStream s {arr};
        s.setByteOrder(QDataStream::LittleEndian);

        s >> ticks;
        s >> mbarc;
        s >> gps.accuracy >> gps.lat >> gps.lon >> gps.ground_speed >> gps.heading ;
        s >> acc.x >> acc.y >> acc.z;
    }

    QString toString() const {
        return QString("{\n  timestamp: %1\n  baro: %2\n  gps: %3\n  accel:  %4\n}").arg(ticks).arg(mbarc).arg(gps.toString()).arg(acc.toString());
    }
};

#endif // SENSOR_PACKET_H
