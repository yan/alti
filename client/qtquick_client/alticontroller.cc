#include "alticontroller.h"
#include "datareader.h"
#include "globals.h"
#include "settings.h"
#include "altisensorpacket.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothUuid>
#include <QDebug>

#include <map>
#include <tuple>

static size_t storage_size = 0;

AltiController::AltiController(QObject *parent)
    : QObject(parent)
    , m_bleDeviceController(nullptr)
    , m_reader(nullptr)
    , rx(new Receiver)
    , m_inProgress(false)
{

    m_services.insert(kAltiServiceUuids.kDeviceInfo.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kConfig.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kUartDebug.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kTime.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kPressure.kService, nullptr);
    m_services.insert(kAltiServiceUuids.kBattery.kService, nullptr);

    m_deviceDiscoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    // m_deviceDiscoveryAgent->setInquiryType(QBluetoothDeviceDiscoveryAgent::LimitedInquiry);

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this,                   &AltiController::deviceDiscovered);

    // Connected via the old way since QBluetoothDeviceDiscoveryAgent::error is overloaded and
    // needs a cast.
    connect(m_deviceDiscoveryAgent, SIGNAL(error(QBluetoothDeviceDiscoveryAgent::Error)),
            this,                   SLOT(deviceScanError(QBluetoothDeviceDiscoveryAgent::Error)));

    connect(m_deviceDiscoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            [](){
       qDebug() << "Scan finished";
    });

    rx->onEventHeader([](struct event_header_s &header) {
        qDebug() << "Received {";
        qDebug() << " event id: " << header.event_id;
        qDebug() << "  samples: " << header.samples;
        qDebug() << "} ( storage: " << header.samples * header.sample_size + sizeof(header) << ")";
        storage_size += header.samples * header.sample_size + sizeof(header);
    });

    rx->onSensorPacket([](struct sensor_packet_s &packet) {
        qDebug() << "Got sensor packet " << sizeof(packet);
    });

}


AltiController::~AltiController()
{
    stopStreamingSensors();
    delete m_deviceDiscoveryAgent;
}

void
AltiController::deviceDiscovered(const QBluetoothDeviceInfo& device)
{

    if ((device.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration) == 0 ||
            device.name() != kName) {
        qDebug() << "Discovered: " << device.name() << " " << device.address().toString() << " " << device.deviceUuid().toString();
        return;
    }

    qDebug() << "Got Alti";

    discoverServices(device);
}

void
AltiController::tryConnect(void)
{
    m_deviceDiscoveryAgent->start();
}

void
AltiController::tryDisconnect(void)
{
    m_deviceDiscoveryAgent->stop();
    if (m_bleDeviceController) {
        delete m_bleDeviceController;
        m_bleDeviceController = nullptr;
    }
}

void
AltiController::deviceScanError(QBluetoothDeviceDiscoveryAgent::Error err)
{
    qDebug() << "Scan error: " << err;
}

/**
 * Called when we know that an Alti device was discovered. Registers AltiController::ble* slots.
 *
 * @brief AltiController::discoverServices
 * @param device
 */
void
AltiController::discoverServices(const QBluetoothDeviceInfo& device)
{
    if (m_bleDeviceController) {
        delete m_bleDeviceController;
    }

    qDebug() << "Discovering services..";

    m_bleDeviceController = QLowEnergyController::createCentral(device, this);

    connect(m_bleDeviceController, &QLowEnergyController::connected,
            [this](){
                    qDebug() << "Discovering services..";
                    m_bleDeviceController->discoverServices();
                });

    connect(m_bleDeviceController,  &QLowEnergyController::serviceDiscovered,
            this,                   &AltiController::bleServiceDiscovered);

    connect(m_bleDeviceController, &QLowEnergyController::discoveryFinished,
            this,                  &AltiController::bleFinished);

    m_bleDeviceController->connectToDevice();

    qDebug() << "Low Energy device found. Scanning for more...";
    }


/**
 * Called when service details are discovered; probably batched. The only way we know
 * this process is done is when all the services we're expecting are discovered.
 *
 * @brief AltiController::serviceDetailsDiscovered
 * @param state
 */
void
AltiController::serviceDetailsDiscovered(QLowEnergyService::ServiceState state)
{
    if (state != QLowEnergyService::ServiceDiscovered) {
        return;
    }

    QLowEnergyService *service = qobject_cast<QLowEnergyService *>(sender());
    if (!service)
        return;

    qDebug() << "serviceDetailsDiscovered " << service->serviceUuid().toString();

    m_services[service->serviceUuid()] = service;

    bool initialized = true;

    for(auto e : m_services.keys())
    {
        if (!m_services.value(e)) {
            initialized = false;
            break;
        }
        if (m_services.value(e)->state() != QLowEnergyService::ServiceDiscovered) {
            initialized = false;
            break;
        }
    }

    if (initialized) {
        qDebug() << "Done";
        waitForMessages();

        // startStreamingSensors();
        emit deviceFinished();
    }

}

void
AltiController::bleServiceDiscovered(QBluetoothUuid uuid)
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



}

void AltiController::bleFinished(void)
{
    qDebug() << "Finished";
}

void AltiController::receivedNotification(QLowEnergyCharacteristic ch, QByteArray arr)
{
    (void) ch;
    if (m_reader == nullptr)
        return;

    m_reader->receivedBytes(arr);
}

void
AltiController::startStreamingSensors(void)
{
    auto svc = m_services[kAltiServiceUuids.kSensorData.kService];
    if (!svc)
        return;

    connect(svc,  SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
            this, SLOT(receivedNotification(QLowEnergyCharacteristic,QByteArray)));

    auto ch = svc->characteristic(kAltiServiceUuids.kSensorData.kDataStream);

    auto desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

    if (!desc.isValid())
        return;

    if (m_reader)
        delete m_reader;

    m_reader = new PacketReader(this);
    m_reader->waitForSensorPacket();

    connect(m_reader, &PacketReader::gotSensorPacket,
        [this](sensor_packet_s pkt) {
            AltiSensorPacket obj(pkt);
            // qDebug() << to_s(pkt);
            emit gotSensors(AltiSensorPacket(obj));
        }
    );

    svc->writeDescriptor(desc, kEnableNotificationsDesc);

    qDebug() << "Subscribed to config data";


}

void
AltiController::stopStreamingSensors(void)
{
    qDebug() << "Stopping streaming..";
    auto svc = m_services[kAltiServiceUuids.kSensorData.kService];
    if (!svc)
        return;

    auto ch = svc->characteristic(kAltiServiceUuids.kSensorData.kDataStream);
    auto desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

    if (desc.isValid()) {
        svc->writeDescriptor(desc, kDisableNotificationsDesc);

        disconnect(m_reader, SIGNAL(gotSensorPacket(sensor_packet_s)));
        delete m_reader;
        m_reader = nullptr;

        disconnect(svc,  SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),
                   this, SLOT(receivedNotification(QLowEnergyCharacteristic,QByteArray)));
    }
}

void
AltiController::sendConfigPacket(struct config_packet_s &packet)
{
    auto svc = m_services[kAltiServiceUuids.kConfig.kService];
    if (!svc)
        return;

    if (m_inProgress)
        return;

    //waitForMessages();

    QByteArray arr(reinterpret_cast<const char *>(&packet), sizeof(packet));
    qDebug() << "Sending: " << arr.toHex();
    auto ch = svc->characteristic(kAltiServiceUuids.kConfig.kRx);
    svc->writeCharacteristic(ch, arr);
    m_inProgress = true;
}

void
AltiController::startRecordingEvent(void)
{
    struct config_packet_s packet;
    memset(&packet, '\0', sizeof(packet));
    packet.type = CONFIG_START_LOGGING;
    sendConfigPacket(packet);
}

void
AltiController::stopRecordingEvent(void)
{
    struct config_packet_s packet;
    memset(&packet, '\0', sizeof(packet));
    packet.type = CONFIG_STOP_LOGGING;
    sendConfigPacket(packet);
}

void
AltiController::formatStorage(void)
{
    struct config_packet_s packet;
    memset(&packet, '\0', sizeof(packet));
    packet.type = CONFIG_FORMAT_STORAGE;
    sendConfigPacket(packet);
}

void
AltiController::waitForMessages()
{
    auto svc = m_services[kAltiServiceUuids.kConfig.kService];
    if (!svc)
        return;

//    auto connection =
     connect(svc, &QLowEnergyService::characteristicChanged,
            [svc, this](QLowEnergyCharacteristic ch,QByteArray arr) {
                    (void) ch;
                    rx->receivedBytes(arr);
                } );

    rx->onConfigPacket([=](struct config_packet_s &packet) {
        QByteArray arr((char*)&packet, sizeof(packet));

        qDebug() << "Total storage space taken: " << storage_size;
        storage_size = 0;
        m_inProgress = false;

//        auto ch = svc->characteristic(kAltiServiceUuids.kConfig.kTx);
//        auto desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
//        if (!desc.isValid())
//            return;
//        svc->writeDescriptor(desc, kDisableNotificationsDesc);

//        disconnect(connection);
//        rx->reset();
    });


    auto ch = svc->characteristic(kAltiServiceUuids.kConfig.kTx);
    auto desc = ch.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
    if (!desc.isValid())
        return;
    svc->writeDescriptor(desc, kEnableNotificationsDesc);
    qDebug() << "Subscribed to config data";

}

void
AltiController::getEvents(void)
{
    struct config_packet_s packet;
    memset(&packet, '\0', sizeof(packet));
    packet.type = CONFIG_LIST_EVENTS;
    sendConfigPacket(packet);
}
