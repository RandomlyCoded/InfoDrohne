#ifndef BTDRONE_H
#define BTDRONE_H

#include "drone.h"

#include <QFile>

namespace randomly {

class BtDrone : public randomly::Drone
{
    Q_OBJECT

public:
    explicit BtDrone(QObject *parent = nullptr);

    // Drone interface
public slots:
    bool sendCommands();
    void start();
    void stop();

private:
    bool m_running = false;
    QTimer *m_sendTimer = nullptr;

    QFile *m_socket;
};

} // namespace randomly

#endif // BTDRONE_H
