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
    m_receiveSocket->bind(m_port + 1); // 2026 12/3 (Tutorium, cooler Zufall)

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

    connect(m_udpTimer, &QTimer::timeout, this, &Drone::sendCommands);
    m_udpTimer->setInterval(Drone::globalInterval());
}

bool UdpDrone::sendCommands()
{
    const auto payload = preparePayload();

    return m_sendSocket->writeDatagram(payload, m_drone, m_port) == propCount() * sizeof(quint16);
}

} // namespace randomly
