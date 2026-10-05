#include "app/controller/device/device_manager.h"

#include "app/controller/device/device_data_parser.h"
#include "app/controller/device/device_poller.h"

#include <algorithm>
#include <limits>

namespace Controller::Device {

DeviceManager::DeviceManager(IDeviceTransport* transport, QObject* parent)
    : QObject(parent)
    , m_transport(transport)
{
    if (!m_transport) {
        return;
    }

    connect(
        m_transport,
        &IDeviceTransport::devicesReceived,
        this,
        &DeviceManager::handleDevicesReceived);
    connect(
        m_transport,
        &IDeviceTransport::pollReceived,
        this,
        &DeviceManager::handlePollReceived);
    connect(
        m_transport,
        &IDeviceTransport::requestFailed,
        this,
        &DeviceManager::handleTransportFailure);
}

void DeviceManager::setPoller(DevicePoller* poller)
{
    if (m_poller == poller) {
        return;
    }

    if (m_poller) {
        disconnect(m_poller, nullptr, this, nullptr);
        m_poller->stop();
    }

    m_poller = poller;
    if (!m_poller) {
        return;
    }

    connect(m_poller, &DevicePoller::pollRequested, this, &DeviceManager::requestPoll);
    if (m_deviceListReady) {
        m_poller->start();
    }
}

qint64 DeviceManager::lastSequence() const
{
    return m_lastSequence;
}

void DeviceManager::initialize()
{
    if (!m_transport) {
        return;
    }

    if (m_activeDevicesRequest) {
        m_activeDevicesRequest.reset();
        m_transport->abortRequest(RequestKind::Devices);
    }

    const RequestId requestId = nextRequestId();
    m_activeDevicesRequest = requestId;
    m_transport->requestDevices(requestId);
}

void DeviceManager::refresh()
{
    if (m_poller) {
        m_poller->restart();
    }

    requestPoll();
}

void DeviceManager::handleDevicesReceived(RequestId requestId, const QByteArray& body)
{
    if (!m_activeDevicesRequest || requestId != *m_activeDevicesRequest) {
        return;
    }

    m_activeDevicesRequest.reset();

    const std::optional<Domain::DeviceList> devices = DeviceDataParser::parseDevices(body);
    if (!devices) {
        return;
    }
    
    m_deviceListReady = true;
    emit devicesUpdated(*devices);
    if (m_poller) {
        m_poller->start();
    }

    requestPoll();
}

void DeviceManager::handlePollReceived(RequestId requestId, const QByteArray& body)
{
    if (!m_activePollRequest || requestId != *m_activePollRequest) {
        return;
    }

    m_activePollRequest.reset();

    const std::optional<Domain::DeviceEventList> events = DeviceDataParser::parsePoll(body);
    if (!events) {
        return;
    }

    Domain::DeviceEventList acceptedEvents = acceptEvents(*events);
    if (!acceptedEvents.isEmpty()) {
        emit eventsAccepted(acceptedEvents);
    }
}

void DeviceManager::handleTransportFailure(const TransportError& error)
{
    std::optional<RequestId>* activeRequest = nullptr;
    if (error.requestKind == RequestKind::Devices) {
        activeRequest = &m_activeDevicesRequest;
    } else {
        activeRequest = &m_activePollRequest;
    }

    if (!*activeRequest || error.requestId != **activeRequest) {
        return;
    }

    activeRequest->reset();
    if (error.code != TransportErrorCode::Aborted) {
        emit transportFailed(error);
    }
}

RequestId DeviceManager::nextRequestId()
{
    const RequestId requestId = m_nextRequestId;
    if (m_nextRequestId == std::numeric_limits<RequestId>::max()) {
        m_nextRequestId = 1;
    } else {
        ++m_nextRequestId;
    }

    return requestId;
}

void DeviceManager::requestPoll()
{
    if (!m_transport) {
        return;
    }

    if (m_activePollRequest) {
        m_activePollRequest.reset();
        m_transport->abortRequest(RequestKind::Poll);
    }

    const RequestId requestId = nextRequestId();
    m_activePollRequest = requestId;
    m_transport->requestPoll(requestId, m_lastSequence);
}

Domain::DeviceEventList DeviceManager::acceptEvents(const Domain::DeviceEventList& events)
{
    Domain::DeviceEventList orderedEvents = events;
    std::stable_sort(
        orderedEvents.begin(),
        orderedEvents.end(),
        [](const Domain::DeviceEvent& left, const Domain::DeviceEvent& right) {
            return left.sequence < right.sequence;
        }
    );

    Domain::DeviceEventList acceptedEvents;
    acceptedEvents.reserve(orderedEvents.size());
    for (const Domain::DeviceEvent& event : orderedEvents) {
        if (event.sequence <= m_lastSequence) {
            continue;
        }

        acceptedEvents.append(event);
        m_lastSequence = event.sequence;
    }

    return acceptedEvents;
}

}  // namespace Controller::Device
