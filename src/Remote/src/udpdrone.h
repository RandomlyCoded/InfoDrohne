#ifndef UDPDRONE_H
#define UDPDRONE_H

#include "drone.h"

namespace randomly {

class UdpDrone : public randomly::Drone
{
    Q_OBJECT

public:
    explicit UdpDrone(QObject *parent = nullptr);

    // Drone interface
public slots:
    bool sendCommands();
    void start() { m_sendTimer->start(); }
    void stop()  { m_sendTimer->stop (); }

private:
    const QHostAddress m_drone = QHostAddress("192.168.4.1"); // 127.0.4.0
    const quint16 m_port = 26122; // Abi 26, Q 12/2

    QUdpSocket *m_sendSocket;
    QUdpSocket *m_receiveSocket;
};

} // namespace randomly

#endif // UDPDRONE_H
