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
    switch (m_protocol) {
    case BtLE:
        return m_btleDrone;
    case Bt:
        return m_btDrone;
    case Udp:
        return m_udpDrone;
    }

    Q_ASSERT_X(false, Q_FUNC_INFO, "Undefined protocol");
}

Backend *Backend::instance()
{
    return __instance;
}

void Backend::toggleDebug()
{
    m_debugMode ^= true;
    emit debugModeChanged();
}

void Backend::setProtocol(const Protocol &newProtocol)
{
    if (m_protocol == newProtocol)
        return;
    m_protocol = newProtocol;
    emit protocolChanged();
}

} // namespace randomly
