#ifndef BTDRONE_H
#define BTDRONE_H

#include "drone.h"

#include <QFile>

namespace randomly {

class BtDrone : public Drone
{
    Q_OBJECT

public:
    explicit BtDrone(QObject *parent = nullptr);

    // Drone interface
public slots:
    bool sendCommand(randomly::Drone::Command type);
    void start() { m_sendTimer->start(); }
    void stop() { m_sendTimer->stop(); }

private:
    bool m_running = false;

    QFile *m_socket;
};

} // namespace randomly

#endif // BTDRONE_H
