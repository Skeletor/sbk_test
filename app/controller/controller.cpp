#include "app/controller/controller.h"

#include "app/config/modules/controller/controller_config.h"
#include "app/controller/device/device_freshness_sentinel.h"
#include "app/controller/device/device_manager.h"
#include "app/controller/device/device_poller.h"
#include "app/controller/device/http_device_transport.h"

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

    m_deviceTransport = new Device::HttpDeviceTransport(m_config.httpDeviceTransport(), this);
    m_devicePoller = new Device::DevicePoller(m_config.devicePoller(), this);
    m_deviceManager = new Device::DeviceManager(m_deviceTransport, this);
    m_deviceFreshnessSentinel = new Device::DeviceFreshnessSentinel(m_config.deviceFreshness(), this);
    m_deviceManager->setPoller(m_devicePoller);

    connect(
        m_deviceManager,
        &Device::DeviceManager::eventsAccepted,
        m_deviceFreshnessSentinel,
        &Device::DeviceFreshnessSentinel::processEvents);

    connect(m_deviceManager, &Device::DeviceManager::devicesUpdated, this, &Controller::devicesUpdated);
    connect(m_deviceManager, &Device::DeviceManager::eventsAccepted, this, &Controller::eventsAccepted);
    connect(m_deviceManager, &Device::DeviceManager::transportFailed, this, &Controller::transportFailed);

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
    if (m_devicePoller) {
        m_devicePoller->stop();
    }

    if (m_deviceTransport) {
        m_deviceTransport->abortRequest(Device::RequestKind::Devices);
        m_deviceTransport->abortRequest(Device::RequestKind::Poll);
    }

    if (m_deviceFreshnessSentinel) {
        m_deviceFreshnessSentinel->clear();
    }
}

}  // namespace Controller
