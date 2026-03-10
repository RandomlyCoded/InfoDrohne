#ifndef BACKEND_H
#define BACKEND_H

#include "btdrone.h"
#include "btledrone.h"
#include "udpdrone.h"

#include <QObject>

namespace randomly {

class Backend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(randomly::Drone *drone READ currentDrone NOTIFY droneChanged FINAL)
    Q_PROPERTY(bool debugMode READ debugMode NOTIFY debugModeChanged FINAL)

    Q_PROPERTY(Protocol protocol READ protocol WRITE setProtocol NOTIFY protocolChanged FINAL)

public:
    enum Protocol {
        BtLE,
        Bt,
        Udp
    };

    Q_ENUM(Protocol);

    explicit Backend(BtLEDrone *btleDrone, UdpDrone *udpDrone, BtDrone *btDrone, QObject *parent = nullptr);

    Drone *currentDrone() const;
    bool debugMode() const { return m_debugMode; }

    static Backend *instance();

    Protocol protocol() const { return m_protocol; }
    void setProtocol(const Protocol &newProtocol);

public slots:
    void toggleDebug();

    bool hasDebugMode() const {
        return
#if defined QT_DEBUG
            true;
#else
            false;
#endif // defined QT_DEBUG
    }

    Qt::WindowFlags windowFlags() const {
#if defined Q_OS_ANDROID
        return Qt::FramelessWindowHint;
#else
        return Qt::Window;
#endif // Q_OS_ANDROID
    }

signals:
    void droneChanged();
    void debugModeChanged();
    void useBtLEChanged();

    void protocolChanged();

private:
    BtLEDrone *m_btleDrone = nullptr;
    UdpDrone *m_udpDrone = nullptr;
    BtDrone *m_btDrone = nullptr;

    bool m_debugMode = false;
    Protocol m_protocol = Bt;
};

} // namespace randomly

#endif // BACKEND_H
