#ifndef DRONE_H
#define DRONE_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>
#include <QVector3D>

namespace randomly {

class Drone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int propCount READ propCount CONSTANT FINAL)
    Q_PROPERTY(QList<int> throttles READ throttles WRITE setThrottle NOTIFY throttlesChanged FINAL)
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged FINAL)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged FINAL)

public:
    enum class Command : quint8 {
        SetMotionVectors = 0x20,

        OptionSelectUDP = 0x40,
        OptionSelectBLE = 0x41,
        OptionSelectBtC = 0x42, // Bluetooth Classic, duh

        DebugSetThrottle   = 0x80,
        DebugEnableLogging = 0x81
    };

    Q_ENUM(Command);

    explicit Drone(QObject *parent = nullptr);

    constexpr static int globalInterval() { return 20; }
    constexpr static int propCount() { return 4; }
    QList<int> throttles() const { return m_throttles; }

    QVector3D direction() const { return m_direction; }
    void setDirection(QVector3D dir) { m_direction = dir; }

    QVector3D rotation() const { return m_rotation; }
    void setRotation(QVector3D rotation) { m_rotation = rotation; }

// to-be-specified API:
public slots:
    virtual bool sendCommand(randomly::Drone::Command type) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

public slots:
    void setThrottle(const QList<int> throttles);
    void setSingleThrottle(const int index, const int value);
    void forceClampThrottles();

signals:
    void throttlesChanged();

    void rotationChanged();
    void directionChanged();

// internal API for consistent data formats
protected:
    QByteArray preparePayload(Command type);

    void sendControlCommand();

    QTimer *m_sendTimer = nullptr;

private:
    QList<int> m_throttles;

    QVector3D m_direction;
    QVector3D m_rotation;

    void dumpThrottles();
};

} // namespace randomly

#endif // DRONE_H
