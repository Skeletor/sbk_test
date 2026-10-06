#pragma once

#include "app/controller/device/idevice_transport.h"
#include "app/domain/device/device_types.h"

#include <QPointer>

#include <optional>

namespace Controller::Device {

class DevicePoller;

class DeviceManager : public QObject {
    Q_OBJECT

public:
    explicit DeviceManager(IDeviceTransport* transport, QObject* parent = nullptr);

    void setPoller(DevicePoller* poller);

    qint64 lastSequence() const;

public:
    void initialize();
    void refresh();
    void shutdown();

signals:
    void devicesUpdated(const Domain::DeviceList& devices);
    void eventsAccepted(const Domain::DeviceEventList& events);
    void transportFailed(const TransportError& error);

private:
    void handleDevicesReceived(RequestId requestId, const QByteArray& body);
    void handlePollReceived(RequestId requestId, const QByteArray& body);
    void handleTransportFailure(const TransportError& error);

    void requestPoll();
    RequestId nextRequestId();
    Domain::DeviceEventList acceptEvents(const Domain::DeviceEventList& events);

private:
    QPointer<IDeviceTransport> m_transport;
    QPointer<DevicePoller> m_poller;
    bool m_deviceListReady = false;
    qint64 m_lastSequence = 0;
    RequestId m_nextRequestId = 1;
    std::optional<RequestId> m_activeDevicesRequest;
    std::optional<RequestId> m_activePollRequest;
};

}  // namespace Controller::Device
