#include "btledrone.h"

#include <QLoggingCategory>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcBtLEDrone, "randomly.BtLeDrone")

QString deviceId(const QBluetoothDeviceInfo &deviceInfo)
{
    return deviceInfo.address().toString();
}

QString deviceId(const QLowEnergyController *controller)
{
    return controller->remoteAddress().toString();
}

bool shouldSkip(const QBluetoothDeviceInfo &deviceInfo) {
    return deviceInfo.name().replace("-", ":") == deviceId(deviceInfo);
}

}

#define RANDOMLY_UNIMPLEMENTED qCWarning(lcBtLEDrone) << "UNIMPLEMENTED:" <<  Q_FUNC_INFO;

BtLEDrone::BtLEDrone(QObject *parent)
    : randomly::Drone{parent}
    , m_sendTimer(new QTimer{this})
{
    connect(m_deviceDiscovery, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BtLEDrone::onDeviceDiscovered);

    startDiscovery();

    m_sendTimer->setInterval(Drone::globalInterval());

    connect(this, &BtLEDrone::stateChanged, this, &BtLEDrone::onStateChanged);
    connect(m_sendTimer, &QTimer::timeout, this, &BtLEDrone::sendThrottle);
}

void BtLEDrone::onDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo)
{
    if (!shouldSkip(deviceInfo))
        qInfo() << "device discovered:" << deviceInfo.name() << deviceId(deviceInfo);

    if (!deviceInfo.name().contains(u"InfoDrone"))
        return;

    m_deviceDiscovery->stop();

    auto device = QLowEnergyController::createCentral(deviceInfo, this);

    connect(device, &QLowEnergyController::errorOccurred,
            this, [device](auto error) {
                qWarning() << "central error:" << error << device->errorString();
            });

    connect(device, &QLowEnergyController::connected,
            this, std::bind(&BtLEDrone::onDeviceConnected, this, device));
    connect(device, &QLowEnergyController::serviceDiscovered,
            this, std::bind(&BtLEDrone::onServicesDiscovered, this, device));
    connect(device, &QLowEnergyController::disconnected,
            this, &BtLEDrone::startDiscovery);

    device->connectToDevice();
}

void BtLEDrone::onDeviceConnected(QLowEnergyController *device)
{
    qInfo() << "connected:" << deviceId(device);
    device->discoverServices();
    m_state = Connected;
    emit stateChanged();
}

void BtLEDrone::onServicesDiscovered(QLowEnergyController *device)
{
    qInfo() << "services discovered:" << deviceId(device) << device->services();

    if (device->services().contains(DroneUUID::DataId)) {
        const auto service = device->createServiceObject(DroneUUID::DataId, this);
        qInfo() << service->serviceUuid() << service->serviceName();

        connect(service, &QLowEnergyService::errorOccurred,
                this, [service](auto error) {
                    qWarning() << "service:" << error;
                });

        connect(service, &QLowEnergyService::stateChanged,
                this, std::bind(&BtLEDrone::onServiceStateChanged, this, service, std::placeholders::_1));

        connect(service, &QLowEnergyService::characteristicChanged,
                this, [service](const QLowEnergyCharacteristic &characteristic,
                          const QByteArray &value) {
                    qInfo() << "characteristic changed:" << characteristic.uuid()
                    << characteristic.name() << value.toHex(' ');
                });

        connect(service, &QLowEnergyService::characteristicRead,
                this, [service](const QLowEnergyCharacteristic &characteristic,
                          const QByteArray &value) {
                    qInfo() << "characteristic read:" << characteristic.uuid()
                    << characteristic.name() << value.toHex(' ');
                });

        service->discoverDetails();

        m_device = device;
    }
}

void BtLEDrone::onServiceStateChanged(QLowEnergyService *service, QLowEnergyService::ServiceState state)
{
    qInfo() << "service state changed:" << state;

    if (state == QLowEnergyService::RemoteServiceDiscovered) {
        auto tx = service->characteristic(DroneUUID::DataId);

        auto cccd = tx.clientCharacteristicConfiguration();
        if (!cccd.isValid()) {
            // your error handling
            return;
        }

        service->writeDescriptor(cccd, QLowEnergyCharacteristic::CCCDEnableNotification);

        if (!tx.isValid()) {
            qWarning() << "Could not find TX characteristic; aborting";
            return;
        }

        m_service = service;

        service->writeCharacteristic(tx, "Hello Dudes\n");

        m_state = Ready;
        emit stateChanged();
    }
}

void BtLEDrone::startDiscovery()
{
    m_deviceDiscovery->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

    m_state = Discovering;
    emit stateChanged();
}

void BtLEDrone::onStateChanged()
{
    if (!m_running || m_state != Ready) {
        m_sendTimer->stop();
        return;
    }

    // m_running == true && m_state == Ready
    m_sendTimer->start();
}

bool BtLEDrone::sendThrottle()
{
    auto tx = m_service->characteristic(DroneUUID::DataId);
    m_service->writeCharacteristic(tx, prepareThrottlePayload(throttles()), QLowEnergyService::WriteWithoutResponse);

    return false;
}

void BtLEDrone::start()
{
    m_running = true;
    emit stateChanged();
}

void BtLEDrone::stop()
{
    m_running = false;
    emit stateChanged();
}

} // namespace randomly
