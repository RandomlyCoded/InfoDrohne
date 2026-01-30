#ifndef DRONE_H
#define DRONE_H

#include <QNetworkAccessManager>
#include <QObject>

namespace randomly {

class Drone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int propCount READ propCount CONSTANT FINAL)
    Q_PROPERTY(QList<int> throttles READ throttles WRITE setThrottle NOTIFY throttlesUpdated_QML FINAL)

public:
    explicit Drone(QObject *parent = nullptr);

    constexpr int propCount() const { return m_propCount; }

    QList<int> throttles() const;

public slots:
    bool setThrottle(QList<int> throttles);

    bool sendThrottle();
    void setSingleThrottle(int index, int value);

    void forceClampThrottles();

signals:
    void throttlesUpdated_QML();

private:
    QByteArray prepareThrottlePayload(const QList<int> throttles);
    constexpr static int m_propCount = 4; // hardcode for now
    QList<int> m_throttles;

    const QHostAddress m_drone = QHostAddress("192.168.4.1"); // 127.0.4.0
    const quint16 m_port = 80;

    QNetworkAccessManager *m_network;
};

} // namespace randomly

#endif // DRONE_H
