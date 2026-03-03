#include "btdrone.h"

#include <QBluetoothSocket>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>

namespace randomly {

BtDrone::BtDrone(QObject *parent)
    : randomly::Drone{parent}
    , m_sendTimer(new QTimer{this})
{
/*
    QBluetoothSocket *s = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);

    connect(s, &QBluetoothSocket::connected, this, [this, s] () {
        qInfo() << "send";
        s->write(preparePayload());
    });

    s->connectToService(QBluetoothAddress{"00:70:07:B0:A7:AA"}, QBluetoothUuid::ServiceClassUuid::UserData);

    connect(s, &QBluetoothSocket::errorOccurred, this, [this] (QBluetoothSocket::SocketError error) {
        qInfo() << "error: " << error;
    });*/

    struct sockaddr_rc addr = { 0 };
    int s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba("00:70:07:B0:A7:AA", &addr.rc_bdaddr); // potentiell {0x00, 0x70, 0x07, 0xB0, 0xA7, 0xAA}

    if (::connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        qFatal("Connect failed");
    }
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
