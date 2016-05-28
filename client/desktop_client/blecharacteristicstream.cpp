#include <QtGlobal>

#include "blecharacteristicstream.h"

BLECharacteristicStream::BLECharacteristicStream(QObject *parent)
    : QIODevice(parent)
{
    setOpenMode(QIODevice::ReadOnly);

}

qint64
BLECharacteristicStream::readData(char *data, qint64 maxlen)
{
    qint64 n = qMin(maxlen, (qint64) m_storage.length());

    memcpy(data, m_storage.data(), n);

    m_storage.remove(0, n);

    return n;
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
    if (arr.length() == 0)
        return;

    m_storage.append(arr);

    emit bytesWritten(arr.length());
    emit readyRead();

    return;
}
