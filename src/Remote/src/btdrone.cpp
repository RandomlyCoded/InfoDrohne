#include "btdrone.h"

#include <QBluetoothSocket>

namespace randomly {

BtDrone::BtDrone(QObject *parent)
    : randomly::Drone{parent}
    , m_sendTimer(new QTimer{this})
{
    QBluetoothSocket *s = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect(s, &QBluetoothSocket::connected, this, [this, s] () {
        qInfo() << "send";
        s->write(preparePayload({1, 2, 3, 4}));
    });

    s->connectToService(QBluetoothAddress{"00:70:07:B0:A7:AA"}, QBluetoothUuid::ServiceClassUuid::UserData);

    connect(s, &QBluetoothSocket::errorOccurred, this, [this] (QBluetoothSocket::SocketError error) {
        qInfo() << "error: " << error;
    });
}

bool BtDrone::sendCommands()
{
    return false;
}

void BtDrone::start()
{

}

void BtDrone::stop()
{

}

} // namespace randomly
