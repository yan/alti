#ifndef GLOBALS_H
#define GLOBALS_H

#include <QBluetoothUuid>
struct kAltiServiceUuids_t {
    using uuid_t = QBluetoothUuid;

    kAltiServiceUuids_t() { }

    struct Service {
        Service(QString uuid):kService(uuid) { }

        uuid_t kService;
    };

    const struct kDeviceInfo_t : public Service {
        kDeviceInfo_t()
            : Service(QString("00001805-0000-1000-8000-00805f9b34fb")) { }
    } kDeviceInfo;

    const struct kTime_t : public Service {
        kTime_t()
            : Service(QString("0000180a-0000-1000-8000-00805f9b34fb")) { }
    } kTime;

    const struct kBattery_t : public Service {
        kBattery_t()
            : Service(QString("0000180f-0000-1000-8000-00805f9b34fb")) { }
    } kBattery;

    const struct kPressure_t : public Service {
        kPressure_t()
            : Service(QString("701b0001-9ddc-4053-bc77-410a972965f7")) { }
    } kPressure;

    const struct kConfig_t : public Service {
        kConfig_t()
            : Service(QString("701b0004-9ddc-4053-bc77-410a972965f7"))
            , kTx(QString("701b0005-9ddc-4053-bc77-410a972965f7"))
            , kRx(QString("701b0006-9ddc-4053-bc77-410a972965f7")) { }

        const uuid_t kTx;
        const uuid_t kRx;
    } kConfig;

    const struct kSensorData_t : public Service {
        kSensorData_t()
            : Service(QString("{701b0001-9ddc-4053-bc77-410a972965f7}"))
            , kDataStream(QString("{701b0002-9ddc-4053-bc77-410a972965f7}")) { }

        const uuid_t kDataStream;
    } kSensorData;

    const struct kUartDebug_t : public Service {
        kUartDebug_t()
            : Service    (QString("{701b0003-9ddc-4053-bc77-410a972965f7}"))
            , kTx        (QString("{6e400003-b5a3-f393-e0a9-e50e24dcca9e}"))
            , kRx        (QString("{6e400002-b5a3-f393-e0a9-e50e24dcca9e}"))
            , kControl   (QString("{6e400004-b5a3-f393-e0a9-e50e24dcca9e}"))
            , kLinkTiming(QString("{6e400005-b5a3-f393-e0a9-e50e24dcca9e}")) { }

        const uuid_t kTx;
        const uuid_t kRx;
        const uuid_t kControl;
        const uuid_t kLinkTiming;
    } kUartDebug;
};

extern const struct kAltiServiceUuids_t kAltiServiceUuids;

#endif // GLOBALS_H
