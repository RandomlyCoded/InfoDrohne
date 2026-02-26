#ifndef BTLEDRONE_H
#define BTLEDRONE_H

#include "drone.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>

namespace randomly {

namespace DroneUUID {

const auto DataId = QBluetoothUuid{u"12345678-90ab-cdef-fedcba09876543210"};

}

class BtLEDrone : public randomly::Drone
{
    Q_OBJECT

public:
    explicit BtLEDrone(QObject *parent = nullptr);

    enum State {
        Discovering,
        Connected,
        Ready,
        None
    };

    // Drone interface
public slots:
    bool sendThrottle();
    void start();
    void stop();

signals:
    void stateChanged();

private:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void onDeviceConnected(QLowEnergyController *device);
    void onServicesDiscovered(QLowEnergyController *device);
    void onServiceStateChanged(QLowEnergyService *service, QLowEnergyService::ServiceState state);

    void onStateChanged();

    void startDiscovery();

    QBluetoothDeviceDiscoveryAgent *const m_deviceDiscovery = new QBluetoothDeviceDiscoveryAgent{this};

    State m_state = None;
    bool m_running = false;
    QLowEnergyService *m_service = nullptr;
    QLowEnergyController *m_device = nullptr;

    QTimer *m_sendTimer = nullptr;
};

} // namespace randomly

#endif // BTLEDRONE_H
