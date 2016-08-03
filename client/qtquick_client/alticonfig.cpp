#include "alticonfig.h"

AltiConfig::AltiConfig(QObject *parent)
    : AltiConfig(parent, nullptr)
{

}

AltiConfig::AltiConfig(QObject *parent, QLowEnergyService *service)
    : QObject(parent)
    , m_service(service)
{
}
