#ifndef ALTICONFIG_H
#define ALTICONFIG_H

#include <QObject>
#include <QLowEnergyService>

class AltiConfig : public QObject
{
    Q_OBJECT
public:
    explicit AltiConfig(QObject *parent = 0);
    AltiConfig(QObject *parent, QLowEnergyService *service);

signals:
    void configChanged(void);

public slots:
private:
    QLowEnergyService *m_service;
};

#endif // ALTICONFIG_H
