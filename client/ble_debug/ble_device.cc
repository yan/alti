#include "ble_device.h"

BleDevice::BleDevice(QObject *parent)
    : QObject(parent), controller(nullptr)
{
}

BleDevice::~BleDevice()
{
    if (controller) {
            delete controller;
    }

    qDeleteAll(services);
    services.clear();
}

void
BleDevice::setBluetoothDeviceInfo(QBluetoothDeviceInfo info)
{
   if (info == this->info) {
       return;
   }

   this->info = info;
   updateServices();
}

void BleDevice::updateServices()
{
   if (!controller) {
       controller = new QLowEnergyController(this->info, this);
   }

   connect(controller, &QLowEnergyController::connected,
           [&]() {
               controller->discoverServices();
   });
   connect(controller, &QLowEnergyController::serviceDiscovered,
           [&](const QBluetoothUuid &uuid) {
              QLowEnergyService *service = controller->createServiceObject(uuid);
              if (!service) {
                  qWarning() << "Can not create service object";
                  return;
              }
              if (service->serviceUuid() == QUuid("701B0001-9DDC-4053-BC77-410A972965F7")) {
                  qDebug() << "Correct service";
              }
              services.append(service);

              qDebug() << "Discovered service: " << service->serviceName();
              //qDebug() << " --- " << *service;
   });

   controller->setRemoteAddressType(QLowEnergyController::PublicAddress);
   controller->connectToDevice();

   emit deviceChanged();
}

