#include "devicemanager.h"
#include <QDebug>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QDataStream>

#include "alti.h"

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
DeviceManager::addDevice(const QBluetoothDeviceInfo &info)
{

    if (info.name() != Alti::Name)
        return;

    qDebug() << "discovered [" << info.name() << "]" << discoveryAgent->discoveredDevices().count();
    qDebug() << " UUID: " << info.deviceUuid();
    qDebug() << " rssi: " << info.rssi();

   // if (!ble_controller)
    ble_controller = new QLowEnergyController(info, this);

    connect(ble_controller, &QLowEnergyController::connected,
            [&]() {
        ble_controller->discoverServices();
    });

    connect(ble_controller, &QLowEnergyController::serviceDiscovered,
            [&](const QBluetoothUuid &uuid) {
        QLowEnergyService *service = ble_controller->createServiceObject(uuid);
        if (!service) {
            return;
        }

        qDebug() << service->serviceName();
        qDebug() << service->serviceUuid();

        if (service->serviceUuid() == QUuid("701b0001-9ddc-4053-bc77-410a972965f7")) {

            connect(service, &QLowEnergyService::stateChanged, this, &DeviceManager::serviceStateChanged);

            qDebug() << "before " << service->characteristics().count();
            service->discoverDetails();
            qDebug() << "Correct service";
        } else {
            qDebug() << "Discovered service.";
        }
    });

    ble_controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
    ble_controller->connectToDevice();
}

void
DeviceManager::serviceStateChanged(QLowEnergyService::ServiceState newState)
{
    qDebug() << "State changed: " << newState;
    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;
    qDebug() << "!";
    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    foreach (const QLowEnergyCharacteristic &ch, chars) {
        if (ch.uuid() == QUuid("701b0002-9ddc-4053-bc77-410a972965f7")) {
            foreach (const QLowEnergyDescriptor &desc,  ch.descriptors()) {
                connect(service, &QLowEnergyService::characteristicChanged,
                        this, &DeviceManager::characteristicChanged);
                service->writeDescriptor(desc, QByteArray::fromHex("1111"));
            }
        }
        qDebug() << ch.name() << ch.uuid();
    }
    //qDebug() << "in state changed" << service->characteristics().count();

}
void
DeviceManager::characteristicChanged(const QLowEnergyCharacteristic &ch, const QByteArray &arr)
{
    QDataStream s {arr};
    uint32_t i;

    s.setByteOrder(QDataStream::LittleEndian);
    s >> i;
    qDebug() << ch.uuid()<< i;

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
