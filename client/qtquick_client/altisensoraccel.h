#if 0
#ifndef __AltiSensorAccel_H
#define __AltiSensorAccel_H
#include <sample.h>
#include <QObject>

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
private:
    qint16 m_x, m_y, m_z;
};

Q_DECLARE_METATYPE(AltiSensorAccel)

#endif
#endif
