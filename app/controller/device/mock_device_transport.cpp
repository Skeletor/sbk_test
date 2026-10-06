#include "app/controller/device/mock_device_transport.h"

#include <QRandomGenerator>
#include <QTimer>

#include <utility>

namespace {

constexpr auto ResponseDelayMs = 25;
constexpr auto ResponseTimeoutMs = 5000;
constexpr auto EventGenerationIntervalMs = 1000;
constexpr auto MaximumStoredEvents = 1000;
constexpr auto NoResponseChance = 0.1;

const auto RoomDeviceId = QStringLiteral("dev-17");
const auto RoomDeviceName = QStringLiteral("Room sensor");
const auto OutsideDeviceId = QStringLiteral("dev-42");
const auto OutsideDeviceName = QStringLiteral("Outside sensor");
const auto TemperatureMetric = QStringLiteral("temperature");
const auto HumidityMetric = QStringLiteral("humidity");

QString getValueMessage(const QString& metric, double value)
{
    return QStringLiteral("%1 = %2").arg(metric).arg(value);
}

}  // namespace

namespace Controller::Device {

MockDeviceTransport::MockDeviceTransport(QObject* parent)
    : IDeviceTransport(parent)
    , m_eventTimer(new QTimer(this))
{
    populateInitialEvents();
    m_eventTimer->setInterval(EventGenerationIntervalMs);
    connect(m_eventTimer, &QTimer::timeout, this, &MockDeviceTransport::populateNextEvent);
    m_eventTimer->start();
}

void MockDeviceTransport::populateNextEvent()
{
    switch (m_generationStep % 8) {
    case 0:
        m_roomTemperature += 0.1;
        appendValueEvent(RoomDeviceId, TemperatureMetric, m_roomTemperature);
        break;

    case 1:
        m_roomHumidity += 0.3;
        appendValueEvent(RoomDeviceId, HumidityMetric, m_roomHumidity);
        break;

    case 2:
        m_outsideTemperature -= 0.2;
        appendValueEvent(OutsideDeviceId, TemperatureMetric, m_outsideTemperature);
        break;

    case 3:
        appendStatusEvent(
            OutsideDeviceId,
            Domain::EventType::Offline,
            QStringLiteral("no data for 60 s"));
        break;

    case 4:
        m_roomTemperature -= 0.2;
        appendValueEvent(RoomDeviceId, TemperatureMetric, m_roomTemperature);
        break;

    case 5:
        appendStatusEvent(OutsideDeviceId, Domain::EventType::Online, QStringLiteral("connection restored"));
        break;

    case 6:
        m_outsideTemperature += 0.4;
        appendValueEvent(OutsideDeviceId, TemperatureMetric, m_outsideTemperature);
        break;

    case 7:
        m_roomHumidity -= 0.2;
        appendValueEvent(RoomDeviceId, HumidityMetric, m_roomHumidity);
        break;
    }

    ++m_generationStep;
}

void MockDeviceTransport::requestDevices(RequestId requestId)
{
    startRequest(RequestKind::Devices, requestId);
}

void MockDeviceTransport::requestPoll(RequestId requestId, qint64 since)
{
    startRequest(RequestKind::Poll, requestId, since);
}

void MockDeviceTransport::abortRequest(RequestKind requestKind)
{
    std::optional<RequestId>& activeRequest = getActiveRequest(requestKind);
    if (!activeRequest) {
        return;
    }

    const RequestId requestId = *activeRequest;
    activeRequest.reset();
    emit requestFailed({
        requestId,
        requestKind,
        TransportErrorCode::Aborted,
        0,
        QStringLiteral("Mock request was aborted"),
    });
}

void MockDeviceTransport::startRequest(RequestKind requestKind, RequestId requestId, qint64 since)
{
    std::optional<RequestId>& activeRequest = getActiveRequest(requestKind);
    if (activeRequest) {
        emit requestFailed({
            requestId,
            requestKind,
            TransportErrorCode::Aborted,
            0,
            QStringLiteral("Another mock request of this kind is already active"),
        });
        return;
    }

    activeRequest = requestId;
    if (QRandomGenerator::global()->generateDouble() < NoResponseChance) {
        scheduleNoResponseTimeout(requestKind, requestId);
        return;
    }

    scheduleResponse(requestKind, requestId, since);
}

void MockDeviceTransport::scheduleNoResponseTimeout(RequestKind requestKind, RequestId requestId)
{
    QTimer::singleShot(
        ResponseTimeoutMs,
        this,
        [this, requestKind, requestId]() {
            std::optional<RequestId>& currentRequest = getActiveRequest(requestKind);
            if (!currentRequest || *currentRequest != requestId) {
                return;
            }

            currentRequest.reset();
            const QString message = requestKind == RequestKind::Devices
                ? QStringLiteral("Mock devices request timed out")
                : QStringLiteral("Mock poll request timed out");

            emit requestFailed({
                requestId,
                requestKind,
                TransportErrorCode::Timeout,
                0,
                message,
            });
        }
    );
}

void MockDeviceTransport::scheduleResponse(RequestKind requestKind, RequestId requestId, qint64 since)
{
    QTimer::singleShot(
        ResponseDelayMs,
        this,
        [this, requestKind, requestId, since]() {
            std::optional<RequestId>& currentRequest = getActiveRequest(requestKind);
            if (!currentRequest || *currentRequest != requestId) {
                return;
            }

            currentRequest.reset();

            if (requestKind == RequestKind::Devices) {
                emit topologyReceived(requestId, getTopology());
            } else {
                emit eventBatchReceived(requestId, getEventBatch(since));
            }
        }
    );
}

DeviceTopology MockDeviceTransport::getTopology() const
{
    return {{
        {RoomDeviceId, RoomDeviceName},
        {OutsideDeviceId, OutsideDeviceName},
    }};
}

DeviceEventBatch MockDeviceTransport::getEventBatch(qint64 since) const
{
    DeviceEventBatch batch;
    batch.lastSequence = m_lastSequence;
    for (const Domain::DeviceEvent& event : m_events) {
        if (event.sequence > since) {
            batch.events.append(event);
        }
    }

    return batch;
}

void MockDeviceTransport::populateInitialEvents()
{
    appendValueEvent(RoomDeviceId, TemperatureMetric, m_roomTemperature);
    appendValueEvent(RoomDeviceId, HumidityMetric, m_roomHumidity);
    appendValueEvent(OutsideDeviceId, TemperatureMetric, m_outsideTemperature);
}

void MockDeviceTransport::appendValueEvent(const QString& deviceId, const QString& metric, double value)
{
    appendEvent({
        ++m_lastSequence,
        QTime::currentTime(),
        deviceId,
        Domain::EventType::Value,
        metric,
        value,
        getValueMessage(metric, value),
    });
}

void MockDeviceTransport::appendStatusEvent(const QString& deviceId, Domain::EventType type, const QString& message)
{
    appendEvent({
        ++m_lastSequence,
        QTime::currentTime(),
        deviceId,
        type,
        std::nullopt,
        std::nullopt,
        message,
    });
}

void MockDeviceTransport::appendEvent(Domain::DeviceEvent event)
{
    m_events.append(std::move(event));

    if (m_events.size() > MaximumStoredEvents) {
        m_events.removeFirst();
    }
}

std::optional<RequestId>& MockDeviceTransport::getActiveRequest(RequestKind requestKind)
{
    return requestKind == RequestKind::Devices
        ? m_activeDevicesRequest
        : m_activePollRequest;
}

}  // namespace Controller::Device
