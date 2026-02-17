#include "udpdrone.h"

#include <QLoggingCategory>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcUdpDrone, "randomly.UdpDrone")

}

UdpDrone::UdpDrone(QObject *parent)
    : randomly::Drone{parent}
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

            qCDebug(lcUdpDrone) << "[DEBUG] Received UDP datagram:" << datagram;
        }
    });

    connect(m_udpTimer, &QTimer::timeout, this, &Drone::sendThrottle);
    m_udpTimer->setInterval(10);
    m_udpTimer->start();
}

bool UdpDrone::sendThrottle()
{
    const auto payload = prepareThrottlePayload(throttles());

    return m_sendSocket->writeDatagram(payload, m_drone, m_port) == propCount() * sizeof(quint16);
}

} // namespace randomly
