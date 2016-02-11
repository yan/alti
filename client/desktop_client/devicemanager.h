#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QList>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QLowEnergyController>

//#include "ble_device.h"
//#include "blecharacteristicstream.h"

class DeviceManager : public QObject
{
    Q_OBJECT
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QLowEnergyController *ble_controller;
    QList<QLowEnergyService*> m_services;
    //BLECharacteristicStream m_stream;
    //QList<BleDevice*> devices;

public:
    DeviceManager():DeviceManager(nullptr){}
    DeviceManager(QObject *parent);

    ~DeviceManager();

public slots:
    void start();
    void stop() ;

signals:
    void bleDataReceived(const QByteArray &arr);

private slots:
    void characteristicChanged(const QLowEnergyCharacteristic &ch, const QByteArray &arr);
    void serviceDiscovered(const QBluetoothUuid &uuid);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void addDevice(const QBluetoothDeviceInfo &);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
};

#endif // DEVICEMANAGER_H

