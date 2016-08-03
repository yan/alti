#if 0
#ifndef ALTISENSORGPS__H
#define ALTISENSORGPS__H
#include <sample.h>
#include <QObject>

class AltiSensorGps {
    Q_GADGET

    Q_PROPERTY(double latitude MEMBER m_lat)
    Q_PROPERTY(double longitude MEMBER m_lon)
    Q_PROPERTY(qint32 accuracy MEMBER m_acc)
    Q_PROPERTY(qint32 speed MEMBER m_groundspeed)
    Q_PROPERTY(qint32 heading MEMBER m_heading)
    Q_PROPERTY(QGeoCoordinate coord READ asCoordinate)

public:
    AltiSensorGps()
        : m_lat(0.0) , m_lon(0.0) , m_acc(0) , m_groundspeed(0) , m_heading(0) { }

    AltiSensorGps(const gps_sample_s &sample)
        : m_lat(sample.lat / static_cast<double>(100.0))
        , m_lon(sample.lon / static_cast<double>(100.0))
        , m_acc(sample.accuracy)
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
                 m_acc == rhs.m_acc &&
                 m_groundspeed == rhs.m_groundspeed &&
                 m_heading == rhs.m_heading);
    }
private:
    double m_lat, m_lon;
    qint32 m_acc, m_groundspeed, m_heading;
};

Q_DECLARE_METATYPE(AltiSensorGps)
#endif // ALTISENSORGPS__H

#endif
