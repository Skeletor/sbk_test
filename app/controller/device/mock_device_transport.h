#pragma once

#include "app/controller/device/idevice_transport.h"

#include <QQueue>

#include <optional>

namespace Controller::Device {

struct MockTransportResponse {
    QByteArray body;
    std::optional<TransportError> error;
};

class MockDeviceTransport : public IDeviceTransport {
    Q_OBJECT

public:
    explicit MockDeviceTransport(QObject* parent = nullptr);

    void enqueueDevicesResponse(MockTransportResponse response);
    void enqueuePollResponse(MockTransportResponse response);

public:
    void requestDevices(RequestId requestId) override;
    void requestPoll(RequestId requestId, qint64 since) override;
    void abortRequest(RequestKind requestKind) override;

private:
    void startRequest(RequestKind requestKind, RequestId requestId);
    QQueue<MockTransportResponse>& getResponses(RequestKind requestKind);
    std::optional<RequestId>& getActiveRequest(RequestKind requestKind);

private:
    QQueue<MockTransportResponse> m_devicesResponses;
    QQueue<MockTransportResponse> m_pollResponses;
    std::optional<RequestId> m_activeDevicesRequest;
    std::optional<RequestId> m_activePollRequest;
};

}  // namespace Controller::Device

Q_DECLARE_METATYPE(Controller::Device::MockTransportResponse)
