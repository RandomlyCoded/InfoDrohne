#ifndef BACKEND_H
#define BACKEND_H

#include "btledrone.h"
#include "udpdrone.h"

#include <QObject>

namespace randomly {

class Backend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(randomly::Drone *drone READ currentDrone NOTIFY droneChanged FINAL)
    Q_PROPERTY(bool debugMode READ debugMode NOTIFY debugModeChanged FINAL)
    Q_PROPERTY(bool useBtLE READ useBtLE NOTIFY useBtLEChanged FINAL)

public:
    explicit Backend(BtLEDrone *btleDrone, UdpDrone *udpDrone, QObject *parent = nullptr);

    Drone *currentDrone() const;
    bool debugMode() const { return m_debugMode; }
    bool useBtLE() const { return m_useBtLE; }

public slots:
    void switchBtLE(bool useBtLE);
    void toggleDebug();

signals:
    void droneChanged();
    void debugModeChanged();
    void useBtLEChanged();

private:
    BtLEDrone *m_btleDrone = nullptr;
    UdpDrone *m_udpDrone = nullptr;

    bool m_debugMode = false;
    bool m_useBtLE = true;
};

} // namespace randomly

#endif // BACKEND_H
