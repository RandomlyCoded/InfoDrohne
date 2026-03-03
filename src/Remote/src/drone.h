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
    Q_PROPERTY(QList<int> throttles READ throttles WRITE setThrottle NOTIFY throttlesUpdated_QML FINAL)
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged FINAL)
    Q_PROPERTY(QVector3D rotation READ rotation WRITE setRotation NOTIFY rotationChanged FINAL)

public:
    explicit Drone(QObject *parent = nullptr);

    constexpr static int globalInterval() { return 500; }
    constexpr int propCount() const { return m_propCount; }
    QList<int> throttles() const { return m_throttles; }

    QVector3D direction() const { return m_direction; }
    void setDirection(QVector3D dir) { m_direction = dir; }

    QVector3D rotation() const { return m_rotation; }
    void setRotation(QVector3D rotation) { m_rotation = rotation; }

// to-be-specified API:
public slots:
    virtual bool sendCommands() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;

public slots:
    void setThrottle(const QList<int> throttles);
    void setSingleThrottle(const int index, const int value);
    void forceClampThrottles();

signals:
    void throttlesUpdated_QML();

    void rotationChanged();
    void directionChanged();

// internal API for consistent data formats
protected:
    QByteArray preparePayload();

private:
    constexpr static int m_propCount = 4; // hardcode for now
    QList<int> m_throttles;

    QVector3D m_direction;
    QVector3D m_rotation;
};

} // namespace randomly

#endif // DRONE_H
