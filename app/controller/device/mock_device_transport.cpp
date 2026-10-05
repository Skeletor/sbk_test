#include "app/controller/device/mock_device_transport.h"

#include <QTimer>

#include <utility>

namespace Controller::Device {

MockDeviceTransport::MockDeviceTransport(QObject* parent)
    : IDeviceTransport(parent)
{
}

void MockDeviceTransport::enqueueDevicesResponse(MockTransportResponse response)
{
    m_devicesResponses.enqueue(std::move(response));
}

void MockDeviceTransport::enqueuePollResponse(MockTransportResponse response)
{
    m_pollResponses.enqueue(std::move(response));
}

void MockDeviceTransport::requestDevices(RequestId requestId)
{
    startRequest(RequestKind::Devices, requestId);
}

void MockDeviceTransport::requestPoll(RequestId requestId, qint64)
{
    startRequest(RequestKind::Poll, requestId);
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

void MockDeviceTransport::startRequest(RequestKind requestKind, RequestId requestId)
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
    QQueue<MockTransportResponse>& responses = getResponses(requestKind);
    MockTransportResponse response;
    if (responses.isEmpty()) {
        response.error = TransportError{
            requestId,
            requestKind,
            TransportErrorCode::Network,
            0,
            QStringLiteral("No mock response is queued"),
        };
    } else {
        response = responses.dequeue();
    }

    QTimer::singleShot(0, this, [this, requestKind, requestId, response = std::move(response)]() mutable {
        std::optional<RequestId>& currentRequest = getActiveRequest(requestKind);
        if (!currentRequest || *currentRequest != requestId) {
            return;
        }

        currentRequest.reset();

        if (response.error) {
            response.error->requestId = requestId;
            response.error->requestKind = requestKind;
            emit requestFailed(*response.error);
        } else if (requestKind == RequestKind::Devices) {
            emit devicesReceived(requestId, response.body);
        } else {
            emit pollReceived(requestId, response.body);
        }
    });
}

QQueue<MockTransportResponse>& MockDeviceTransport::getResponses(RequestKind requestKind)
{
    return requestKind == RequestKind::Devices
        ? m_devicesResponses
        : m_pollResponses;
}

std::optional<RequestId>& MockDeviceTransport::getActiveRequest(RequestKind requestKind)
{
    return requestKind == RequestKind::Devices
        ? m_activeDevicesRequest
        : m_activePollRequest;
}

}  // namespace Controller::Device
