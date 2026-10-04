#pragma once

#include "app/config/modules/device/device_freshness_config.h"
#include "app/config/modules/device/device_poller_config.h"
#include "app/config/modules/device/http_device_transport_config.h"

class QSettings;

namespace Config::Controller {

class ControllerConfig {
public:
    explicit ControllerConfig(const QSettings& settings);

    const Device::HttpDeviceTransportConfig& httpDeviceTransport() const;
    const Device::DevicePollerConfig& devicePoller() const;
    const Device::DeviceFreshnessConfig& deviceFreshness() const;

private:
    Device::HttpDeviceTransportConfig m_httpDeviceTransport;
    Device::DevicePollerConfig m_devicePoller;
    Device::DeviceFreshnessConfig m_deviceFreshness;
};

}  // namespace Config::Controller
