#include "app/config/modules/controller/controller_config.h"

namespace Config::Controller {

ControllerConfig::ControllerConfig(QSettings& settings)
    : m_httpDeviceTransport(settings)
    , m_devicePoller(settings)
    , m_deviceFreshness(settings)
    , m_deviceTransport(settings)
{
}

const Device::HttpDeviceTransportConfig& ControllerConfig::httpDeviceTransport() const
{
    return m_httpDeviceTransport;
}

const Device::DevicePollerConfig& ControllerConfig::devicePoller() const
{
    return m_devicePoller;
}

const Device::DeviceFreshnessConfig& ControllerConfig::deviceFreshness() const
{
    return m_deviceFreshness;
}

const Device::DeviceTransportConfig& ControllerConfig::deviceTransport() const
{
    return m_deviceTransport;
}

}  // namespace Config::Controller
