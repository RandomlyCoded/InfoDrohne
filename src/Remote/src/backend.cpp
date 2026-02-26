#include "backend.h"

namespace randomly {

Backend::Backend(BtLEDrone *btleDrone, UdpDrone *udpDrone, QObject *parent)
    : QObject(parent)
    , m_btleDrone(btleDrone)
    , m_udpDrone(udpDrone)
{
    currentDrone()->start();
}

Drone *Backend::currentDrone() const
{
    if (m_useBtLE)
        return m_btleDrone;

    return m_udpDrone;
}

void Backend::switchBtLE(bool useBtLE)
{
    currentDrone()->stop();
    m_useBtLE = useBtLE;
    currentDrone()->start();

    emit droneChanged();
    emit useBtLEChanged();
}

void Backend::toggleDebug()
{
    m_debugMode ^= true;
    emit debugModeChanged();
}

} // namespace randomly
