#ifndef BTDRONE_H
#define BTDRONE_H

#include "drone.h"

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
};

} // namespace randomly

#endif // BTDRONE_H
