#include "devicemanager.h"
#include <QDebug>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QDataStream>

#include "alti.h"

const static QUuid kStreamCharacteristicUuid("701b0002-9ddc-4053-bc77-410a972965f7");

DeviceManager::DeviceManager(QObject *parent)
    :QObject(parent)
    ,ble_controller(nullptr)
{
    qDebug() << "starting";

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    discoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::LimitedInquiry);

    connect(
      discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
      this,           &DeviceManager::addDevice);

    // error() is overloaded, using method pointers requires casting
    connect(
       discoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
       this,           SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));

    connect(
       discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
       this,           &DeviceManager::deviceScanFinished);
}

DeviceManager::~DeviceManager()
{
  qDeleteAll(devices);

  delete discoveryAgent;
}

void
DeviceManager::serviceDiscovered(const QBluetoothUuid &uuid)
{
    QLowEnergyService *service = ble_controller->createServiceObject(uuid);
    if (!service) {
        return;
    }

    qDebug() << service->serviceName() << " " << service->serviceUuid();

    if (service->serviceUuid() == QUuid("701b0001-9ddc-4053-bc77-410a972965f7")) {
        connect(service, &QLowEnergyService::stateChanged,
                this, &DeviceManager::serviceStateChanged);

        qDebug() << "before " << service->characteristics().count();
        service->discoverDetails();
        qDebug() << "Correct service";
    } else {
        qDebug() << "Discovered service.";
    }

}

void
DeviceManager::addDevice(const QBluetoothDeviceInfo &info)
{

    if (info.name() != Alti::Name)
        return;

    qDebug() << "discovered [" << info.name() << "]" << discoveryAgent->discoveredDevices().count();
    qDebug() << " UUID: " << info.deviceUuid();
    qDebug() << " rssi: " << info.rssi();

    // We found a device
    ble_controller = new QLowEnergyController(info, this);

    // Once connected, ask to discovery its services
    connect(ble_controller, &QLowEnergyController::connected,
            [&]() { ble_controller->discoverServices(); });

    // Handle discovered servies
    connect(ble_controller, &QLowEnergyController::serviceDiscovered,
            this, &DeviceManager::serviceDiscovered);

    // Start the discovery
    ble_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
    ble_controller->connectToDevice();
}

void
DeviceManager::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    qDebug() << "State changed: " << newState;
    if (newState != QLowEnergyService::ServiceDiscovered)
        return;

    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    foreach (const QLowEnergyCharacteristic &ch, chars) {
        if (ch.uuid() == kStreamCharacteristicUuid) {
            QLowEnergyDescriptor notification =
                    ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

            if (!notification.isValid())
                return;

            connect(service, &QLowEnergyService::characteristicChanged,
                    this,    &DeviceManager::characteristicChanged);
            service->writeDescriptor(notification, QByteArray::fromHex("ffff"));
            qDebug() << "Wrote descriptor";

        }
        qDebug() << ch.name() << ch.uuid();
    }

}


void
DeviceManager::characteristicChanged(const QLowEnergyCharacteristic &ch, const QByteArray &arr)
{
    if (ch.uuid() == kStreamCharacteristicUuid) {
        qDebug() << "characteristic value: " << arr.toHex().toStdString().c_str();

        QDataStream s {arr};
        uint32_t i;

        s.setByteOrder(QDataStream::LittleEndian);
        s >> i;

        qDebug() << " value: " << i;
    }

}

void
DeviceManager::deviceScanFinished()
{
    qDebug() << "Finished. Found: " << discoveryAgent->discoveredDevices().count();
}

void
DeviceManager::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error err)
{
    qDebug() << "Got error: " << err << "\n";
}

void
DeviceManager::start()
{
    discoveryAgent->start();
}
