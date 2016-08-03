#ifndef DATAREADER_H
#define DATAREADER_H


#include <QDebug>
#include <QObject>
#include <QDataStream>
#include <QByteArray>

#include <vector>
#include <sample.h>
#include <settings.h>
#include "altisensorpacket.h"


using HeaderType = uint8_t;


class Receiver {

protected:
    QByteArray _buffer;


    uint8_t peekHeader(void) {
        HeaderType header = 0;

        if ((size_t)_buffer.size() < sizeof(HeaderType))
            return 0;

        for (size_t i = 0; i < sizeof(HeaderType); i++) {
            header = (header << 8) | _buffer.at((int)i);
        }

        return header;
    }

public:

    Receiver()
        : _spCallback([](sensor_packet_s &obj){ qDebug() << sizeof(obj); })
        , _cpCallback([](config_packet_s &obj){ qDebug() << sizeof(obj); })
        , _ehCallback([](event_header_s &obj){ qDebug() << sizeof(obj); })
    {

    }

    void reset(void) {
        _buffer.clear();
    }

    void receivedBytes(const QByteArray &arr) {
        qDebug() << "Received " << arr.size() << " bytes";
        _buffer.append(arr);

        attempt();
    }

    virtual void attempt(void) {
        if (peekHeader() == CONFIG_RESPONSE_MSG) {
            config_packet_s pkt;
            if (read(pkt))
                _cpCallback(pkt);
        } else if (peekHeader() == CONFIG_RESPONSE_EVENT) {
            event_header_s pkt;
            if (read(pkt))
                _ehCallback(pkt);
        } else if (peekHeader() == CONFIG_RESPONSE_SAMPLE) {
            sensor_packet_s pkt;
            if (read(pkt))
                _spCallback(pkt);
        }
    }

    void onSensorPacket(std::function<void(sensor_packet_s&)> cb) {
        _spCallback = cb;
    }

    void onConfigPacket(std::function<void(config_packet_s&)> cb) {
        _cpCallback = cb;
    }
    void onEventHeader(std::function<void(event_header_s&)> cb) {
        _ehCallback = cb;
    }

private:
    template <typename T>
    bool read(T &dest) {
        if ((size_t)_buffer.size() < sizeof(T) + sizeof(HeaderType))
            return false;

        QDataStream s(_buffer);
        T obj;
        HeaderType header;
        int bytesRead = 0;

        qDebug() << _buffer.toHex();
        bytesRead += s.readRawData(reinterpret_cast<char*>(&header), sizeof(header));
        qDebug() << "Read " << bytesRead << " bytes (header = " << header << ")";
        bytesRead += s.readRawData(reinterpret_cast<char*>(&obj), sizeof(T));
        qDebug() << "Read " << bytesRead << " bytes";

        _buffer.remove(0, bytesRead);
        dest = obj;
        return true;
    }

    std::function<void(sensor_packet_s&)> _spCallback;
    std::function<void(config_packet_s&)> _cpCallback;
    std::function<void(event_header_s&)> _ehCallback;
};















class ByteReceiver {
    static constexpr char LEAD_BYTE = 0xff;
public:
    ByteReceiver() {
    }

    void receivedBytes(const QByteArray &arr) {
        _buffer.append(arr);

        if (_buffer.size() >= size()) {
            char c;
            do {
                if (_buffer.isEmpty())
                    break;

                c = _buffer.at(0);
                _buffer.remove(0, 1);

            } while (c != LEAD_BYTE);


            if (_buffer.size() < size()) {
                return;
            }

            QDataStream s(_buffer);
            getObject(s);
            _buffer.remove(0, size());
        }
    }

    bool ready(void) {
        return size() >= _buffer.size();
    }

protected:
    virtual int size(void) const = 0;

    virtual void getObject(QDataStream &s) = 0;

    QByteArray _buffer;
};

class PacketReader : public QObject, public ByteReceiver {
    Q_OBJECT

    enum {
        kUndefined,
        kSensorPacket,
        kConfigResponse
    } _expecting;

public:
    PacketReader(QObject *parent):QObject(parent), _expecting(kSensorPacket/*kUndefined*/) { }

    void waitForConfigResponse() {
        _expecting = kConfigResponse;
    }

    void waitForSensorPacket() {
        _expecting = kSensorPacket;
    }

private:

    int size(void) const override {
        switch (_expecting) {
        case kSensorPacket:
            return (int) sizeof(sensor_packet_s);
        case kConfigResponse:
            return (int) sizeof(setting_value_s);
        default:
            return 0;
        }
    }

    void getObject(QDataStream &s) override {
        switch (_expecting) {
        case kSensorPacket: {
            sensor_packet_s obj;
            s >> obj;
            emit gotSensorPacket(obj);
        }

        case kConfigResponse: {
            config_packet_s obj;
            s >> obj;
            emit gotConfigPacket(obj);
        }
        case kUndefined: {
        }

        }

    }

signals:
    void gotSensorPacket(sensor_packet_s obj);

    void gotConfigPacket(config_packet_s obj);
};

#endif // DATAREADER_H
