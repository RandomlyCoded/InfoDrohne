#ifndef DRONE_H
#define DRONE_H

#include <QObject>

namespace randomly {

class Drone : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int propCount READ propCount CONSTANT FINAL)

public:
    explicit Drone(QObject *parent = nullptr);

    int propCount() const;

signals:

private:
    int m_propCount = 4; // hardcode for now
};

} // namespace randomly

#endif // DRONE_H
