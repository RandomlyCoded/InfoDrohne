#include "drone.h"

#include <QLoggingCategory>
#include <QNetworkReply>

namespace randomly {

namespace {

Q_LOGGING_CATEGORY(lcDrone, "randomly.Drone");

quint16 clamp_u16(int v) {
    return qMin(qMax(0, v), INT16_MAX);
}

} // namespace

Drone::Drone(QObject *parent)
    : QObject{parent}
    , m_network(new QNetworkAccessManager{this})
    , m_throttles{QList<int>(m_propCount, 0)}
{
    qCInfo(lcDrone) << "init";
    m_network->connectToHost(m_drone.toString(), m_port);
    connect(m_network, &QNetworkAccessManager::finished, this, [this] (QNetworkReply *r){
        qCInfo(lcDrone) << r->request().url().toDisplayString() << ":" << r->error();
    });

    connect(m_network, &QNetworkAccessManager::sslErrors, this, [this] (const QNetworkReply *r, QList<QSslError> errors) {
        qCInfo(lcDrone) << errors;
    });
}

int Drone::propCount() const
{
    return m_propCount;
}

bool Drone::setThrottle(QList<int> throttles)
{
    Q_ASSERT_X(throttles.size() == m_propCount, Q_FUNC_INFO, "throttle count missmatch");

    m_throttles = throttles;
    emit throttlesUpdated_QML();

    qCDebug(lcDrone) << "setting" << throttles;

    return sendThrottle();
}

bool Drone::sendThrottle()
{
    const static auto url = QUrl{QString("http://%1:%2/throttle").arg(m_drone.toString(), QString::number(m_port))};
    auto req = QNetworkRequest(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "raw/uint16[]");

    const auto payload = prepareThrottlePayload(m_throttles);
    m_network->post(req, payload);
    qCInfo(lcDrone) << payload.toHex('.');

    return true;
}

void Drone::setSingleThrottle(int index, int value)
{
    m_throttles[index] = value;

    // no update since this is only intended to be called from QML
    // but we need to send the updated throttle to the drone
    maybeSendThrottle();
}

bool Drone::maybeSendThrottle()
{
    if (false) // TODO: timing logic, e.g. wait like 10 ms between sending data
        return false;

    return sendThrottle();
}

void Drone::forceClampThrottles()
{
    for (auto &t: m_throttles)
        t = clamp_u16(t);
}

QByteArray Drone::prepareThrottlePayload(const QList<int> throttles)
{
    QByteArray payload;
    QDataStream s{&payload, QIODevice::WriteOnly};

    payload.reserve(m_propCount * sizeof(quint16));
    s.setByteOrder(QDataStream::LittleEndian);

    // writing the list directly would add separators etc.
    for (const auto t: throttles) {
        s << clamp_u16(t);
    }

    return payload;
}

QList<int> Drone::throttles() const
{
    return m_throttles;
}

} // namespace randomly
