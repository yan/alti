#include "blecharacteristicstream.h"

BLECharacteristicStream::BLECharacteristicStream(QObject *parent)
    :QIODevice(parent)
{

}

qint64
BLECharacteristicStream::readData(char *data, qint64 maxlen)
{
    (void) data;
    (void) maxlen;
    return 0;
}

qint64
BLECharacteristicStream::writeData(const char *data, qint64 len)
{
    (void) data;
    (void) len;
    return 0;
}

void
BLECharacteristicStream::receivedBytes(QByteArray arr)
{
    (void) arr;
    return;
}
