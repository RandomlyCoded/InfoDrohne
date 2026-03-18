#ifndef BTLEDRONE_H
#define BTLEDRONE_H

#include "drone.h"

#include <QBluetoothDeviceDiscoveryAgent>
#include <QLowEnergyController>

namespace randomly {

namespace DroneUUID {

const auto ServiceId  = QBluetoothUuid{u"69fc8256-d3ca-9112-bce0-388de24860d3"};
const auto ThrottleId = QBluetoothUuid{u"ebc49f11-9ffc-96b2-2a4e-6994a99175b5"};

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
    Q_ENUM(State);

    // Drone interface
public slots:
    bool sendCommands();
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
};

} // namespace randomly

#endif // BTLEDRONE_H
