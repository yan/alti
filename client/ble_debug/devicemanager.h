#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QList>

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QLowEnergyController>

#include "ble_device.h"

class DeviceManager : public QObject
{
    Q_OBJECT
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QLowEnergyController *ble_controller;
    QList<BleDevice*> devices;

public:
    DeviceManager():DeviceManager(nullptr){}
    DeviceManager(QObject *parent);

    ~DeviceManager();
    void start();

//signals:

private slots:
    void characteristicChanged(const QLowEnergyCharacteristic &ch, const QByteArray &arr);
    void serviceDiscovered(const QBluetoothUuid &uuid);
    void serviceStateChanged(QLowEnergyService::ServiceState newState);
    void addDevice(const QBluetoothDeviceInfo &);
    void deviceScanFinished();
    void deviceScanError(QBluetoothDeviceDiscoveryAgent::Error);
};

#endif // DEVICEMANAGER_H
