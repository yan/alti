#ifndef ALTIDEVICE_H
#define ALTIDEVICE_H

#include <QObject>
#include <QLowEnergyService>
#include <QBluetoothUuid>
#include <QLowEnergyController>
#include <QBluetoothDeviceInfo>
#include <QMap>
#include <QVariantMap>

#include "datareader.h"

#include <memory>

class AltiDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ deviceName)
public:
    explicit AltiDevice(QObject *parent = 0);
    AltiDevice(QObject *parent, const QBluetoothDeviceInfo &info);
    ~AltiDevice();


signals:

    void gotSensors(AltiSensorPacket packet);
    void deviceReady();

public slots:
    void startStreamingSensors(void);
    void stopStreamingSensors(void);

    void startRecordingEvent(void);
    void stopRecordingEvent(void);

    void addService(QBluetoothUuid uuid);
    void serviceDetailsDiscovered(QLowEnergyService::ServiceState state);

    void discoverServices();

    // Slots for services
    void receivedNotification(QLowEnergyCharacteristic ch,QByteArray arr);

    void lowEnergyFinished(void);
private:
    /*
    QLowEnergyService *deviceInfo;
    QLowEnergyService *time;
    QLowEnergyService *battery;
    QLowEnergyService *pressure;
    QLowEnergyService *uart;
    QLowEnergyService *config;
    */
    QMap<QBluetoothUuid, QLowEnergyService*> m_services;

    QLowEnergyController* m_bleDeviceController;
    QBluetoothDeviceInfo m_info;
    PacketReader *m_reader;
};

#endif // ALTIDEVICE_H
