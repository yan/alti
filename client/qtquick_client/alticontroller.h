#ifndef ALTICONTROLLER_H
#define ALTICONTROLLER_H

#include <QString>
#include <QObject>
#include <QList>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>
#include <QDataStream>

#include "altidevice.h"

#include <vector>
#include <memory>
#include <cstddef>

#include <settings.h>



struct DeviceInfo: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString deviceName READ getName NOTIFY deviceChanged)
    Q_PROPERTY(QString deviceAddress READ getAddress NOTIFY deviceChanged)
public:
    DeviceInfo(QObject *parent):QObject(parent) { }
    DeviceInfo(const DeviceInfo &rhs):device(rhs.device) { }
    DeviceInfo(const QBluetoothDeviceInfo &d):device(d) { }
    QString getAddress() const {
#ifdef Q_OS_MAC
        return device.deviceUuid().toString();
#else
        return device.address().toString();
#endif
    }
    QString getName() const { return device.name(); }
    QBluetoothDeviceInfo getDevice() { return device; }
    void setDevice(const QBluetoothDeviceInfo &dev) { device = dev; emit deviceChanged(); }


signals:
    void deviceChanged();

private:
    QBluetoothDeviceInfo device;
};

class AltiController : public QObject
{
    Q_OBJECT

    const QString kName = "Aero";
    const QByteArray kEnableNotificationsDesc = QByteArray::fromHex("0100");
    const QByteArray kDisableNotificationsDesc = QByteArray::fromHex("0000");

public:
    explicit AltiController(QObject *parent = 0);
    ~AltiController();

signals:
    void deviceFinished();
    void gotSensors(AltiSensorPacket packet);

public slots:
    void tryConnect(void);
    void tryDisconnect(void);

    // Slots for the ble discovery agent
    void deviceDiscovered(const QBluetoothDeviceInfo& dev);
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error err);


    // Slots for BLE discovery agent
    // void addLowEnergyService(QBluetoothUuid uuid);
    // void bleConnected(void);

    void startStreamingSensors(void);
    void stopStreamingSensors(void);

    void startRecordingEvent(void);
    void stopRecordingEvent(void);

    void selectMostRecent(void);
    void dumpMostRecent(QString file);

    void formatStorage(void);

    void getEvents(void);

    void bleServiceDiscovered(QBluetoothUuid uuid);
    void serviceDetailsDiscovered(QLowEnergyService::ServiceState state);

    void discoverServices(const QBluetoothDeviceInfo& device);

    // Slots for services
    void receivedNotification(QLowEnergyCharacteristic ch,QByteArray arr);

    void bleFinished(void);
protected:
    void sendConfigPacket(struct config_packet_s &packet);

private:
    void waitForMessages(void);
    QMap<QBluetoothUuid, QLowEnergyService*> m_services;

    QLowEnergyController* m_bleDeviceController;
    PacketReader *m_reader;

    QBluetoothDeviceDiscoveryAgent *m_deviceDiscoveryAgent;
    Receiver *rx;
    bool m_inProgress;
};

#endif // ALTICONTROLLER_H
