#pragma once

#include "app/controller/device/idevice_transport.h"
#include "app/domain/device/device_types.h"

#include <QObject>
#include <QStringList>

namespace Config::Controller {
class ControllerConfig;
}

namespace Controller::Device {
class DeviceFreshnessSentinel;
class DeviceManager;
class DevicePoller;
class HttpDeviceTransport;
}  // namespace Controller::Device

namespace Controller {

class Controller : public QObject {
    Q_OBJECT

public:
    explicit Controller(const Config::Controller::ControllerConfig& config, QObject* parent = nullptr);

public:
    void initialize();
    void refreshDevices();
    void shutdown();

signals:
    void initialized();
    void devicesUpdated(const Domain::DeviceList& devices);
    void eventsAccepted(const Domain::DeviceEventList& events);
    void devicesBecameUnreliable(const QStringList& deviceIds);
    void transportFailed(const Device::TransportError& error);

private:
    const Config::Controller::ControllerConfig& m_config;
    Device::HttpDeviceTransport* m_deviceTransport = nullptr;
    Device::DevicePoller* m_devicePoller = nullptr;
    Device::DeviceManager* m_deviceManager = nullptr;
    Device::DeviceFreshnessSentinel* m_deviceFreshnessSentinel = nullptr;
};

}  // namespace Controller
