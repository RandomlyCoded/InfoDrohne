#include "drone.h"

#include <QLoggingCategory>
#include <QNetworkReply>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcDrone, "randomly.Drone");

quint16 clamp_u16(int v) {
    return qMin(qMax(0, v), INT16_MAX);
}

} // namespace

Drone::Drone(QObject *parent)
    : QObject{parent}
    , m_throttles{QList<int>(m_propCount, 0)}
    , m_sendSocket(new QUdpSocket{this})
    , m_receiveSocket(new QUdpSocket{this})
    , m_udpTimer(new QTimer{this})
{
    m_receiveSocket->bind(8081);

    connect(m_receiveSocket, &QUdpSocket::readyRead, this, [&] {
        while (m_receiveSocket->hasPendingDatagrams())
        {
            QByteArray datagram;
            datagram.resize (m_receiveSocket->pendingDatagramSize ());
            QDataStream str (&datagram, QUdpSocket::ReadOnly);
            m_receiveSocket->readDatagram (datagram.data (), datagram.size ());

            qCDebug(lcDrone) << "[DEBUG] Received UDP datagram:" << datagram;
        }
    });

    connect(m_udpTimer, &QTimer::timeout, this, &Drone::sendThrottle);
    m_udpTimer->setInterval(10);
    m_udpTimer->start();
}

void Drone::setThrottle(QList<int> throttles)
{
    Q_ASSERT_X(throttles.size() == m_propCount, Q_FUNC_INFO, "throttle count missmatch");

    m_throttles = throttles;
    emit throttlesUpdated_QML();
}

bool Drone::sendThrottle()
{
    const auto payload = prepareThrottlePayload(m_throttles);

    return m_sendSocket->writeDatagram(payload, m_drone, m_port) == m_propCount * sizeof(quint16);
}

void Drone::setSingleThrottle(int index, int value)
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
    for (const auto t: throttles) {
        s << clamp_u16(t);
    }

    return payload;
}

QList<int> Drone::throttles() const
{
    return m_throttles;
}

} // namespace randomly
