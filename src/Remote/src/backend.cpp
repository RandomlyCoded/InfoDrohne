#include "backend.h"

namespace randomly {

namespace
{

static Backend *__instance = nullptr;

} // namespace

Backend::Backend(BtLEDrone *btleDrone, UdpDrone *udpDrone, BtDrone *btDrone, QObject *parent)
    : QObject(parent)
    , m_btleDrone(btleDrone)
    , m_btDrone(btDrone)
    , m_udpDrone(udpDrone)
{
    Q_ASSERT_X(Backend::instance() == nullptr, Q_FUNC_INFO, "Backend already initialized!");

    __instance = this;

    currentDrone()->start();
}

Drone *Backend::currentDrone() const
{
    qInfo() << (m_useBtLE ? (Drone*)m_btDrone: m_udpDrone);

    if (m_useBtLE)
        return m_btDrone;

    return m_udpDrone;
}

Backend *Backend::instance()
{
    return __instance;
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
