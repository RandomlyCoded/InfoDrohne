#include "backend.h"

namespace randomly {

namespace
{

using namespace Qt::StringLiterals;

constexpr auto key_lowTrims        = u"lowTrims"_sv;
constexpr auto key_highTrims       = u"highTrims"_sv;
constexpr auto key_debug           = u"debug"_sv;
constexpr auto key_protocol        = u"protocol"_sv;
constexpr auto key_embeddedLogging = u"embeddedLogging"_sv;

static Backend *__instance = nullptr;

} // namespace

Backend::Backend(BtLEDrone *btleDrone, UdpDrone *udpDrone, BtDrone *btDrone, QObject *parent)
    : QObject(parent)
    , m_btleDrone(btleDrone)
    , m_btDrone(btDrone)
    , m_udpDrone(udpDrone)
    , m_settings(new QSettings(this))
{
    Q_ASSERT_X(Backend::instance() == nullptr, Q_FUNC_INFO, "Backend already initialized!");

    connect(this, &Backend::protocolChanged, this, &Backend::droneChanged);

    __instance = this;

    resetLowTrims();
    resetHighTrims();
    resetDebugMode();
    resetProtocol();

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

void Backend::setDebugMode(bool newDebugMode)
{
    m_debugMode = newDebugMode;
    m_settings->setValue(key_debug, m_debugMode);

    emit debugModeChanged();
}

void Backend::setProtocol(const Protocol &newProtocol)
{
    if (m_protocol == newProtocol)
        return;

    // update protocol
    switch (newProtocol) {
    case BtLE: currentDrone()->sendCommand(Drone::Command::OptionSelectBLE); break;
    case Bt:   currentDrone()->sendCommand(Drone::Command::OptionSelectBtC); break;
    case Udp:  currentDrone()->sendCommand(Drone::Command::OptionSelectUDP); break;
    }

    currentDrone()->stop();

    m_protocol = newProtocol;
    m_settings->setValue(key_protocol, m_protocol);

    currentDrone()->start();

    emit protocolChanged();
}

void Backend::setLowTrims(const QList<int> &newLowTrims)
{
    if (m_lowTrims == newLowTrims)
        return;

    m_lowTrims = newLowTrims;
    m_settings->setValue(key_lowTrims, QVariant::fromValue(m_lowTrims));

    emit lowTrimsChanged();
}

void Backend::setHighTrims(const QList<int> &newHighTrims)
{
    if (m_highTrims == newHighTrims)
        return;

    m_highTrims = newHighTrims;
    m_settings->setValue(key_highTrims, QVariant::fromValue(m_highTrims));

    emit highTrimsChanged();
}

void Backend::resetLowTrims()
{
    m_lowTrims = qvariant_cast<QList<int>>(m_settings->value(key_lowTrims));
    while (m_lowTrims.size() < Drone::propCount())
        m_lowTrims.append(0);
}

void Backend::resetHighTrims()
{
    m_highTrims = qvariant_cast<QList<int>>(m_settings->value(key_highTrims));
    while (m_highTrims.size() < Drone::propCount())
        m_highTrims.append(0);
}

void Backend::resetDebugMode()
{
    m_debugMode = qvariant_cast<bool>(m_settings->value(key_debug, false));
}

void Backend::resetProtocol()
{
    setProtocol(qvariant_cast<Protocol>(m_settings->value(key_protocol, BtLE)));
}

void Backend::setEmbeddedLogging(bool newEnableEmbeddedLogging)
{
    if (m_enableEmbeddedLogging == newEnableEmbeddedLogging)
        return;

    m_enableEmbeddedLogging = newEnableEmbeddedLogging;
    currentDrone()->sendCommand(Drone::Command::DebugEnableLogging);

    emit enableEmeddedLoggingChanged();
}

void Backend::resetEmbeddedLogging()
{
    setEmbeddedLogging(qvariant_cast<bool>(m_settings->value(key_embeddedLogging, false)));
}

} // namespace randomly
