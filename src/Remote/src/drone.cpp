#include "drone.h"

#include "backend.h"

#include <QLoggingCategory>
#include <QNetworkReply>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcDrone, "randomly.Drone");

quint8 clampU8(int i)
{
    return qMax(qMin(i, UINT8_MAX), 0);
}

} // namespace

Drone::Drone(QObject *parent)
    : QObject{parent}
    , m_throttles{QList<int>(propCount(), 0)}
    , m_sendTimer(new QTimer{this})
{
    m_sendTimer->setSingleShot(false);
    m_sendTimer->setInterval(globalInterval());
}

void Drone::setThrottle(const QList<int> throttles)
{
    Q_ASSERT_X(throttles.size() == propCount(), Q_FUNC_INFO, "throttle count missmatch");

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
        t = clampU8(t);
}

QByteArray Drone::preparePayload()
{
    QByteArray payload;
    QDataStream s{&payload, QIODevice::WriteOnly};
    s.setByteOrder(QDataStream::LittleEndian);

    s << quint8(Backend::instance()->debugMode() << 7); // d000 0000

    if (Backend::instance()->debugMode()) {
        // writing the list directly would add separators etc.
        for (const auto t: m_throttles)
            s << clampU8(t);
    }
    else {
        for (int i = 0; i < 3; ++i)
            s <<  clampU8(m_direction[i]);

        for (int i = 0; i < 3; ++i)
            s <<  clampU8(m_rotation[i]);
    }

    qInfo() << payload.toHex(':');

    return payload;
}

} // namespace randomly
