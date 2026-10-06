#pragma once

#include "app/controller/device/idevice_transport.h"
#include "app/domain/device/device_types.h"

#include <optional>

class QTimer;

namespace Controller::Device {

class MockDeviceTransport : public IDeviceTransport {
    Q_OBJECT

public:
    explicit MockDeviceTransport(QObject* parent = nullptr);

public:
    void requestDevices(RequestId requestId) override;
    void requestPoll(RequestId requestId, qint64 since) override;
    void abortRequest(RequestKind requestKind) override;

private:
    void startRequest(RequestKind requestKind, RequestId requestId, qint64 since = 0);
    void scheduleNoResponseTimeout(RequestKind requestKind, RequestId requestId);
    void scheduleResponse(RequestKind requestKind, RequestId requestId, qint64 since);
    DeviceTopology getTopology() const;
    DeviceEventBatch getEventBatch(qint64 since) const;
    void populateInitialEvents();
    void populateNextEvent();
    void appendEvent(Domain::DeviceEvent event);
    void appendValueEvent(const QString& deviceId, const QString& metric, double value);
    void appendStatusEvent(const QString& deviceId, Domain::EventType type, const QString& message);
    std::optional<RequestId>& getActiveRequest(RequestKind requestKind);

private:
    Domain::DeviceEventList m_events;
    QTimer* m_eventTimer = nullptr;
    qint64 m_lastSequence = 0;
    int m_generationStep = 0;
    double m_roomTemperature = 23.5;
    double m_roomHumidity = 41.2;
    double m_outsideTemperature = 7.0;
    std::optional<RequestId> m_activeDevicesRequest;
    std::optional<RequestId> m_activePollRequest;
};

}  // namespace Controller::Device
