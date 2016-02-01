#ifndef BLE_DEVICE_H
#define BLE_DEVICE_H

#include <QObject>
#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>

class BleDevice : public QObject
{
    Q_OBJECT
    QBluetoothDeviceInfo info;
    QLowEnergyController *controller;
    QList<QLowEnergyService*> services;

public:
    explicit BleDevice(QObject *parent = 0);
    ~BleDevice();

    void updateServices();

    QBluetoothDeviceInfo bluetoothDeviceInfo() const;
    void setBluetoothDeviceInfo(QBluetoothDeviceInfo info);

signals:
    void deviceChanged();

public slots:
};

#endif // BLE_DEVICE_H
