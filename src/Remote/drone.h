#ifndef DRONE_H
#define DRONE_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

namespace randomly {

class Drone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int propCount READ propCount CONSTANT FINAL)
    Q_PROPERTY(QList<int> throttles READ throttles WRITE setThrottle NOTIFY throttlesUpdated_QML FINAL)

public:
    explicit Drone(QObject *parent = nullptr);

    constexpr int propCount() const { return m_propCount; }
    QList<int> throttles() const { return m_throttles; }

public slots:
    void setThrottle(const QList<int> throttles);

    virtual bool sendThrottle() = 0;
    void setSingleThrottle(const int index, const int value);

    void forceClampThrottles();

signals:
    void throttlesUpdated_QML();

// internal API for consistent data formats
protected:
    QByteArray prepareThrottlePayload(const QList<int> throttles);

private:
    constexpr static int m_propCount = 4; // hardcode for now
    QList<int> m_throttles;
};

} // namespace randomly

#endif // DRONE_H
