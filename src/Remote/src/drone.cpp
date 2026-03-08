#include "drone.h"

#include "backend.h"

#include <QLoggingCategory>
#include <QNetworkReply>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcDrone, "randomly.Drone");

const quint16 clamp_u16(const int v) {
    return qMin(qMax(0, v), UINT16_MAX);
}

} // namespace

Drone::Drone(QObject *parent)
    : QObject{parent}
    , m_throttles{QList<int>(m_propCount, 0)}
{}

void Drone::setThrottle(const QList<int> throttles)
{
    Q_ASSERT_X(throttles.size() == m_propCount, Q_FUNC_INFO, "throttle count missmatch");

    m_throttles = throttles;
    emit throttlesUpdated_QML();
}

void Drone::setSingleThrottle(const int index, const int value)
{
    m_throttles[index] = value;
}

void Drone::forceClampThrottles()
{
    for (auto &t: m_throttles)
        t = clamp_u16(t);
}

QByteArray Drone::preparePayload()
{
    QByteArray payload;
    QDataStream s{&payload, QIODevice::WriteOnly};
    s.setByteOrder(QDataStream::LittleEndian);

    s << quint8(128);

    // writing the list directly would add separators etc.
    for (const auto t: throttles())
        s << quint8(clamp_u16(t) >> 8);


    return payload;

    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000
    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000

    return payload;

    if (Backend::instance()->debugMode()) {
        // writing the list directly would add separators etc.
        for (const auto t: m_throttles)
            s << clamp_u16(t);
    }
    else {
        for (int i = 0; i < 3; ++i)
            s <<  clamp_u16(m_direction[i]);

        for (int i = 0; i < 3; ++i)
            s <<  clamp_u16(m_rotation[i]);
    }

    return payload;
}

} // namespace randomly
