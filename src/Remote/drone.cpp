#include "drone.h"

#include <QLoggingCategory>
#include <QNetworkReply>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcDrone, "randomly.Drone");

const quint16 clamp_u16(const int v) {
    return qMin(qMax(0, v), INT16_MAX);
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

QByteArray Drone::prepareThrottlePayload(const QList<int> throttles)
{
    QByteArray payload;
    QDataStream s{&payload, QIODevice::WriteOnly};

    payload.reserve(m_propCount * sizeof(quint16));
    s.setByteOrder(QDataStream::LittleEndian);

    // writing the list directly would add separators etc.
    for (const auto t: throttles)
        s << clamp_u16(t);

    return payload;
}

} // namespace randomly
