#ifndef BLECHARACTERISTICSTREAM_H
#define BLECHARACTERISTICSTREAM_H

#include <QIODevice>
#include <QByteArray>
#include <sample.h>

class BLECharacteristicStream : public QIODevice
{
    QByteArray m_storage;

public:
    BLECharacteristicStream(QObject *parent);


protected:
    virtual qint64 readData(char *data, qint64 maxlen) override;
    virtual qint64 writeData(const char *data, qint64 len) override;

public slots:
    void receivedBytes(QByteArray arr);

};

#endif // BLECHARACTERISTICSTREAM_H
