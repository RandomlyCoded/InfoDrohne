#ifndef BACKEND_H
#define BACKEND_H

#include "btdrone.h"
#include "btledrone.h"
#include "udpdrone.h"

#include <QObject>
#include <QSettings>

namespace randomly {

class Backend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(randomly::Drone *drone READ currentDrone NOTIFY droneChanged FINAL)
    Q_PROPERTY(bool debugMode READ debugMode WRITE setDebugMode RESET resetDebugMode NOTIFY debugModeChanged FINAL)

    Q_PROPERTY(Protocol protocol READ protocol WRITE setProtocol RESET resetProtocol NOTIFY protocolChanged FINAL)
    Q_PROPERTY(QList<int> lowTrims READ lowTrims WRITE setLowTrims RESET resetLowTrims NOTIFY lowTrimsChanged FINAL)
    Q_PROPERTY(QList<int> highTrims READ highTrims WRITE setHighTrims RESET resetHighTrims NOTIFY highTrimsChanged FINAL)

public:
    enum Protocol {
        BtLE,
        Bt,
        Udp
    };

    Q_ENUM(Protocol);

    Backend(const Backend &) = delete;
    Backend(Backend &&) = delete;
    Backend &operator=(const Backend &) = delete;
    Backend &operator=(Backend &&) = delete;
    explicit Backend(BtLEDrone *btleDrone, UdpDrone *udpDrone, BtDrone *btDrone,
                     QObject *parent = nullptr);

    Drone *currentDrone() const;

    bool debugMode() const { return m_debugMode; }
    void setDebugMode(bool newDebugMode);
    void resetDebugMode();

    static Backend *instance();

    Protocol protocol() const { return m_protocol; }
    void setProtocol(const Protocol &newProtocol);

    QList<int> lowTrims() const { return m_lowTrims; }
    void setLowTrims(const QList<int> &newLowTrims);
    void resetLowTrims();

    QList<int> highTrims() const { return m_highTrims; }
    void setHighTrims(const QList<int> &newHighTrims);
    void resetHighTrims();

    void resetProtocol();

public slots:
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

    void lowTrimsChanged();

    void highTrimsChanged();

private:
    BtLEDrone *const m_btleDrone = nullptr;
    UdpDrone  *const m_udpDrone = nullptr;
    BtDrone   *const m_btDrone = nullptr;

    bool m_debugMode = false;
    Protocol m_protocol = BtLE;
    QList<int> m_lowTrims;
    QList<int> m_highTrims;

    QSettings *const m_settings;
};

} // namespace randomly

#endif // BACKEND_H
