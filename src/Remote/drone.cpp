#include "drone.h"

namespace randomly {

Drone::Drone(QObject *parent)
    : QObject{parent}
{}

int Drone::propCount() const
{
    return m_propCount;
}

} // namespace randomly
