#include "devicemanager.h"
#include <QDebug>
#include <QBluetoothDeviceInfo>
#include <QBluetoothUuid>
#include <QDataStream>
#include <QTextStream>
#include "sensor_packet.h"
#include "alti.h"

const static QUuid kBaroStreamServiceUuid    ("701b0001-9ddc-4053-bc77-410a972965f7");
const static QUuid kStreamCharacteristicUuid ("701b0002-9ddc-4053-bc77-410a972965f7");
const static QUuid kDataXferUartServiceUuid  ("701b0003-9ddc-4053-bc77-410a972965f7");
const static QUuid kDataXferUartTxCharUuid   ("6e400003-b5a3-f393-e0a9-e50e24dcca9e");
const static QUuid kDataXferUartRxCharUuid   ("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
const static QUuid kDataXferUartCPCharUuid   ("6e400004-b5a3-f393-e0a9-e50e24dcca9e");
const static QUuid kDataXferUartLtCharUuid   ("6e400005-b5a3-f393-e0a9-e50e24dcca9e");
static QTextStream qout(stdout);

DeviceManager::DeviceManager(QObject *parent)
    :QObject(parent)
    ,ble_controller(nullptr)
{
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

    qDebug() << "connected";
}

DeviceManager::~DeviceManager()
{
  // qDeleteAll(devices);

  delete discoveryAgent;
}

void
DeviceManager::serviceDiscovered(const QBluetoothUuid &uuid)
{
    QLowEnergyService *service = ble_controller->createServiceObject(uuid);
    if (!service) {
        return;
    }
    QString serviceName = "other";

    if (service->serviceUuid() == kBaroStreamServiceUuid) {
        //connect(service, &QLowEnergyService::stateChanged,
        //        this, &DeviceManager::serviceStateChanged);
        //service->discoverDetails();
        //serviceName = "Baro Stream";
    } else if (service->serviceUuid()  == kDataXferUartServiceUuid) {
        connect(service, &QLowEnergyService::stateChanged,
                this, &DeviceManager::serviceStateChanged);
        service->discoverDetails();
        serviceName = "Data xfer";
    }
    qDebug() << "Service discovered: " << serviceName << " " << service->serviceName();
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
    if (newState != QLowEnergyService::ServiceDiscovered)
        return;

    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    qDebug() << "State changed: " << newState << ", service: " << service->serviceName();

    const QList<QLowEnergyCharacteristic> chars = service->characteristics();
    foreach (const QLowEnergyCharacteristic &ch, chars) {
        qDebug() << "Characteristic: " << ch.name() << " " << ch.uuid();
        if (ch.uuid() == kDataXferUartTxCharUuid) {
            QLowEnergyDescriptor notification =
                    ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            qDebug() << "Made a notification " << notification.name() << " (" << notification.isValid() << ")";
            if (!notification.isValid())
                return;

            qDebug() << "Subscribing to : " << ch.uuid();
            connect(service, &QLowEnergyService::characteristicChanged,
                    this,    &DeviceManager::characteristicChanged);

            service->writeDescriptor(notification, QByteArray::fromHex("ffff"));
            m_services.append(service);
            connect(this, &DeviceManager::destroyed, [&]() {
               qDebug() << "Unsub.";
            });

        }
        qDebug() << ch.name() << ch.uuid();
    }

}


void
DeviceManager::characteristicChanged(const QLowEnergyCharacteristic &ch, const QByteArray &arr)
{
    //qDebug() << "changed ..";
    if (ch.uuid() == kDataXferUartTxCharUuid) {
        sensor_packet_s p(arr);
        qout << p.toString() << endl;

#if 0

        //qDebug() << "characteristic value: (" << arr.length() << ")" << arr.toHex().toStdString().c_str();

        QDataStream s {arr};
        uint32_t i;

        s.setByteOrder(QDataStream::LittleEndian);
        s >> i;

        qDebug() << " value: " << i;
#endif
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
    qDebug() << "Starting";
    discoveryAgent->start();
}

void
DeviceManager::stop()
{
    foreach (QLowEnergyService *service, m_services) {
        qDebug() << "Stopping " << service->serviceName();
        const QList<QLowEnergyCharacteristic> chars = service->characteristics();

        foreach (const QLowEnergyCharacteristic &ch, chars) {
            qDebug() << "Characteristic: " << ch.name() << " " << ch.uuid();
            if (ch.uuid() == kStreamCharacteristicUuid) {
                QLowEnergyDescriptor notification =
                        ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

                if (!notification.isValid())
                    return;
                service->writeDescriptor(notification, QByteArray::fromHex("0000"));
            }
        }
    }

    qDeleteAll(m_services);
}
