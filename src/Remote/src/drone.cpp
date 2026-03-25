#include "drone.h"

#include "backend.h"

#include <QLoggingCategory>
#include <QNetworkReply>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcDrone, "randomly.Drone");

quint8 clampU8(int i)
{
    return qMax(qMin(i, UINT8_MAX), 0);
}

void writeMotionVectors(QDataStream &stream, const QVector3D direction, const QVector3D rotation)
{
    for (int i = 0; i < 3; ++i)
        stream <<  clampU8(direction[i]);

    for (int i = 0; i < 3; ++i)
        stream <<  clampU8(rotation[i]);
}

void writeDebugThrottles(QDataStream &stream, const QList<int> &throttles)
{
    // writing the list directly would add separators etc.
    for (const auto t: throttles)
        stream << clampU8(t);
}

void writeLoggingState(QDataStream &stream, const bool logging)
{
    stream << logging;
}

} // namespace

Drone::Drone(QObject *parent)
    : QObject{parent}
    , m_throttles{QList<int>(propCount(), 0)}
    , m_sendTimer(new QTimer{this})
{
    m_sendTimer->setSingleShot(false);
    m_sendTimer->setInterval(globalInterval());
    connect(m_sendTimer, &QTimer::timeout, this, &Drone::sendControlCommand);

    connect(this, &Drone::throttlesChanged, this, &Drone::dumpThrottles);
}

void Drone::setThrottle(const QList<int> throttles)
{
    Q_ASSERT_X(throttles.size() == propCount(), Q_FUNC_INFO, "throttle count missmatch");

    m_throttles = throttles;

    emit throttlesChanged();
}

void Drone::setSingleThrottle(const int index, const int value)
{
    m_throttles[index] = value;
    emit throttlesChanged();
}

void Drone::forceClampThrottles()
{
    for (auto &t: m_throttles)
        t = clampU8(t);
}

QByteArray Drone::preparePayload(Command type)
{
    QByteArray payload;
    QDataStream s{&payload, QIODevice::WriteOnly};
    s.setByteOrder(QDataStream::LittleEndian);

    s << type;

    switch (type) {
    case Command::SetMotionVectors: writeMotionVectors(s, m_direction, m_rotation); break;

    case Command::OptionSelectUDP: break;
    case Command::OptionSelectBLE: break;
    case Command::OptionSelectBtC: break; // no more data required

    case Command::DebugSetThrottle: writeDebugThrottles(s, throttles()); break;
    case Command::DebugEnableLogging: writeLoggingState(s, Backend::instance()->enableEmbeddedLogging()); break;
        break;
    }

    return payload;
}

void Drone::sendControlCommand()
{
    if (Backend::instance()->debugMode())
        sendCommand(Command::DebugSetThrottle);

    else
        sendCommand(Command::SetMotionVectors);
}

void Drone::dumpThrottles()
{
    static QFile dumpFile("throttles.csv");
    static bool open = dumpFile.open(QFile::WriteOnly);
    static QTextStream stream(&dumpFile);

    const auto now = QDateTime::currentDateTimeUtc();
    stream << now.toMSecsSinceEpoch()/1000 << "." << now.toMSecsSinceEpoch() % 1000
           << "|" << m_throttles[0]
           << "|" << m_throttles[1]
           << "|" << m_throttles[2]
           << "|" << m_throttles[3] << Qt::endl;
}

} // namespace randomly
