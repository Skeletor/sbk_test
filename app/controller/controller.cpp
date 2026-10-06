#include "app/controller/controller.h"

#include "app/config/modules/controller/controller_config.h"
#include "app/controller/device/device_freshness_sentinel.h"
#include "app/controller/device/device_manager.h"
#include "app/controller/device/device_poller.h"
#include "app/controller/device/http_device_transport.h"
#include "app/controller/device/mock_device_transport.h"

namespace {

Controller::Device::DeviceOperation getDeviceOperation(
    Controller::Device::RequestKind requestKind)
{
    switch (requestKind) {
    case Controller::Device::RequestKind::Devices:
        return Controller::Device::DeviceOperation::LoadTopology;

    case Controller::Device::RequestKind::Poll:
        return Controller::Device::DeviceOperation::PollEvents;
    }

    return Controller::Device::DeviceOperation::LoadTopology;
}

Controller::Device::DeviceOperationErrorCode getDeviceOperationErrorCode(
    Controller::Device::TransportErrorCode transportErrorCode)
{
    switch (transportErrorCode) {
    case Controller::Device::TransportErrorCode::Network:
    case Controller::Device::TransportErrorCode::HttpStatus:
        return Controller::Device::DeviceOperationErrorCode::Communication;

    case Controller::Device::TransportErrorCode::Timeout:
        return Controller::Device::DeviceOperationErrorCode::Timeout;

    case Controller::Device::TransportErrorCode::InvalidPayload:
        return Controller::Device::DeviceOperationErrorCode::InvalidResponse;

    case Controller::Device::TransportErrorCode::Aborted:
        return Controller::Device::DeviceOperationErrorCode::Cancelled;
    }

    return Controller::Device::DeviceOperationErrorCode::Communication;
}

Controller::Device::DeviceOperationError getDeviceOperationError(
    const Controller::Device::TransportError& transportError)
{
    return {
        getDeviceOperation(transportError.requestKind),
        getDeviceOperationErrorCode(transportError.code),
        transportError.message,
    };
}

}  // namespace

namespace Controller {

Controller::Controller(
    const Config::Controller::ControllerConfig& config,
    QObject* parent)
    : QObject(parent)
    , m_config(config)
{
}

void Controller::initialize()
{
    if (m_deviceManager) {
        return;
    }

    Device::IDeviceTransport* deviceTransport = nullptr;
    switch (m_config.deviceTransport().type()) {
    case Config::Device::DeviceTransportType::Http:
        deviceTransport = new Device::HttpDeviceTransport(m_config.httpDeviceTransport(), this);
        break;

    case Config::Device::DeviceTransportType::Mock:
        deviceTransport = new Device::MockDeviceTransport(this);
        break;
    }

    Device::DevicePoller* devicePoller = new Device::DevicePoller(m_config.devicePoller(), this);
    m_deviceManager = new Device::DeviceManager(deviceTransport, this);
    m_deviceFreshnessSentinel = new Device::DeviceFreshnessSentinel(m_config.deviceFreshness(), this);
    m_deviceManager->setPoller(devicePoller);

    connect(
        m_deviceManager,
        &Device::DeviceManager::eventsAccepted,
        m_deviceFreshnessSentinel,
        &Device::DeviceFreshnessSentinel::processEvents);

    connect(m_deviceManager, &Device::DeviceManager::devicesUpdated, this, &Controller::devicesUpdated);
    connect(m_deviceManager, &Device::DeviceManager::eventsAccepted, this, &Controller::eventsAccepted);
    connect(
        m_deviceManager,
        &Device::DeviceManager::transportFailed,
        this,
        [this](const Device::TransportError& error) {
            emit deviceOperationFailed(getDeviceOperationError(error));
        }
    );

    connect(
        m_deviceFreshnessSentinel,
        &Device::DeviceFreshnessSentinel::devicesBecameUnreliable,
        this,
        &Controller::devicesBecameUnreliable);

    m_deviceManager->initialize();
    emit initialized();
}

void Controller::refreshDevices()
{
    if (m_deviceManager) {
        m_deviceManager->refresh();
    }
}

void Controller::shutdown()
{
    if (m_deviceManager) {
        m_deviceManager->shutdown();
    }

    if (m_deviceFreshnessSentinel) {
        m_deviceFreshnessSentinel->clear();
    }
}

}  // namespace Controller
