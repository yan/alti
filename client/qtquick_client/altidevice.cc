#include <QDebug>
#include <QVariantMap>

#include "altidevice.h"
#include "globals.h"

#include <sample.h>
#include <settings.h>

AltiDevice::AltiDevice(QObject *parent)
    : AltiDevice(parent, QBluetoothDeviceInfo{})
{

}

AltiDevice::AltiDevice(QObject *parent, const QBluetoothDeviceInfo &info)
    : QObject(parent)
    , m_bleDeviceController(nullptr)
    , m_info(info)
{
    m_services.insert(kAltiServiceUuids.kDeviceInfo.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kConfig.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kUartDebug.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kTime.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kPressure.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kBattery.kService, nullptr);
}

AltiDevice::~AltiDevice()
{
    delete m_bleDeviceController;
}

void
AltiDevice::discoverServices()
{
    if (m_bleDeviceController) {
        delete m_bleDeviceController;
    }
    qDebug() << "Discovering services..";
    m_bleDeviceController = QLowEnergyController::createCentral(m_info, this);

    connect(m_bleDeviceController, &QLowEnergyController::connected,
            [this](){
                    qDebug() << "Discovering services..";
                    m_bleDeviceController->discoverServices();
                });
            // m_bleDeviceController, SLOT(discoverServices()));

    connect(m_bleDeviceController, SIGNAL(serviceDiscovered(QBluetoothUuid)),
            this,                  SLOT(addService(QBluetoothUuid)));

    connect(m_bleDeviceController, SIGNAL(discoveryFinished()),
            this,                  SLOT(lowEnergyFinished()));

    m_bleDeviceController->connectToDevice();
    qDebug() << "Low Energy device found. Scanning for more...";
    }

void
AltiDevice::serviceDetailsDiscovered(QLowEnergyService::ServiceState state)
{
    if (state != QLowEnergyService::ServiceDiscovered) {
        return;
    }

    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    qDebug() << "serviceDetailsDiscovered " << service->serviceUuid().toString();

    m_services[service->serviceUuid()] = service;

    int initialized = 1;
    for(auto e : m_services.keys())
    {
        if (!m_services.value(e)) {
            initialized = 0; break;
        }
       if (m_services.value(e)->state() != QLowEnergyService::ServiceDiscovered) {
           initialized = 0; break;
       }
      //fout << e << "," << extensions.value(e) << '\n';
    }
    if (initialized) {
        qDebug() << "Done";
        startStreamingSensors();
        //emit deviceReady();
    }
    return;

    foreach (auto &ch, service->characteristics()) {
        if (ch.uuid() == kAltiServiceUuids.kSensorData.kDataStream) {
            qDebug() << "Got data stream characteristic: " << ch.name() << ", " << ch.uuid();
            const QLowEnergyDescriptor desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
            if (desc.isValid()) {
                qDebug() << "Writing descriptor";
                service->writeDescriptor(desc, QByteArray::fromHex("0100"));
            }
    //     } else if (ch.uuid() == kAltiServiceUuids.kConfig.kRx) {
    //         struct setting_packet_s packet;
    //         packet.type = SETTING_START_LOGGING;
    //         QByteArray arr(reinterpret_cast<const char*>(&packet), sizeof(packet));

    //         service->writeCharacteristic(ch, arr);

    //         qDebug() << "Wrote config characteristic: " << ch.name() << ", " << ch.uuid();
         }
    }
}

void
AltiDevice::addService(QBluetoothUuid uuid)
{
    QLowEnergyService *service = m_bleDeviceController->createServiceObject(uuid, this);
    if (!service) {
        qWarning() << "Can't create service.";
        return;
    }

    if (service->state() == QLowEnergyService::DiscoveryRequired) {
        connect(service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                this,     SLOT(serviceDetailsDiscovered(QLowEnergyService::ServiceState)));

        service->discoverDetails();
    } else {
        // m_services
        m_services[uuid] = service;
    }


    connect(service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this,      SLOT(receivedNotification(QLowEnergyCharacteristic,QByteArray)));

    if (uuid == kAltiServiceUuids.kSensorData.kService) {

        // pressure->discoverDetails();
    }

#if 0
    if (uuid == kAltiServiceUuids.kConfig.kService) {
        qDebug() << "Got config service.";

        config = m_bleDeviceController->createServiceObject(uuid);
        if (!config) {
            qWarning() << "Can't create a config service.";
            return;
        }

        if (config->state() == QLowEnergyService::DiscoveryRequired) {
            connect(config, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),
                    this,   SLOT(serviceDetailsDiscovered(QLowEnergyService::ServiceState)));

            connect(config, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
                    this,   SLOT(receivedNotification(QLowEnergyCharacteristic,QByteArray)));

            config->discoverDetails();
        }
    }
#endif
}

void AltiDevice::lowEnergyFinished(void)
{
    qDebug() << "Finished";
    // emit deviceReady();
}

void AltiDevice::receivedNotification(QLowEnergyCharacteristic ch, QByteArray arr)
{
    (void) ch;
    if (m_reader == nullptr)
        return;

    m_reader->receivedBytes(arr);
}

void
AltiDevice::startStreamingSensors(void)
{
    auto svc = m_services[kAltiServiceUuids.kSensorData.kService];
    if (!svc)
        return;

    auto ch = svc->characteristic(kAltiServiceUuids.kSensorData.kDataStream);
    auto desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (desc.isValid()) {
        if (m_reader)
            delete m_reader;

        m_reader = new PacketReader(this);
        m_reader->waitForSensorPacket();
        connect(m_reader, &PacketReader::gotSensorPacket,
            [this](sensor_packet_s pkt) {
                AltiSensorPacket obj(pkt);
                emit gotSensors(obj);
            }
        );

        svc->writeDescriptor(desc, QByteArray::fromHex("0100"));

        qDebug() << "Subscribed to config data";

    }
}

void
AltiDevice::stopStreamingSensors(void)
{
    auto svc = m_services[kAltiServiceUuids.kSensorData.kService];
    if (!svc)
        return;

    auto ch = svc->characteristic(kAltiServiceUuids.kSensorData.kDataStream);
    auto desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

    if (desc.isValid()) {
        svc->writeDescriptor(desc, QByteArray::fromHex("0000"));
        disconnect(m_reader, SIGNAL(PacketReader::gotSensorPacket));
        delete m_reader;
        m_reader = nullptr;
    }
}

void
AltiDevice::startRecordingEvent(void)
{
    auto svc = m_services[kAltiServiceUuids.kConfig.kService];
    if (!svc)
        return;

    auto ch = svc->characteristic(kAltiServiceUuids.kConfig.kRx);
}

void
AltiDevice::stopRecordingEvent(void)
{

}
