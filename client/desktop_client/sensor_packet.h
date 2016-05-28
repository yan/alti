#ifndef SENSOR_PACKET_H
#define SENSOR_PACKET_H

#include <stdint.h>

//#include <sample.h>
#include <QDataStream>

#if 1

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
                        .arg(lat/1e7f).arg(lon/1e7f).arg(ground_speed/1e3f).arg(heading/1e5f).arg(accuracy / 100.0f);
        }
    } gps_sample;

    struct accel_sample_s {
        int16_t x, y, z;
        QString toString() const {
            return QString("{x: %1, y %2, z: %3}").arg(x).arg(y).arg(z);
        }
    } accel_sample;

    sensor_packet_s(QByteArray arr) {
        QDataStream s {arr};
        s.setByteOrder(QDataStream::LittleEndian);

        s >> ticks;
        s >> mbarc;
        s >> gps_sample.accuracy >> gps_sample.lat >> gps_sample.lon >> gps_sample.ground_speed >> gps_sample.heading ;
        s >> accel_sample.x >> accel_sample.y >> accel_sample.z;
    }

    QString toString() const {
        return QString("{\n  timestamp: %1\n  baro: %2\n  gps: %3\n  accel:  %4\n}").arg(ticks).arg(mbarc/100.0).arg(gps_sample.toString()).arg(accel_sample.toString());
    }
};

#endif

#endif // SENSOR_PACKET_H
