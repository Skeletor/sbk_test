#include "app/config/modules/controller/controller_config.h"

namespace Config::Controller {

ControllerConfig::ControllerConfig(const QSettings& settings)
    : m_httpDeviceTransport(settings)
    , m_devicePoller(settings)
    , m_deviceFreshness(settings)
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

}  // namespace Config::Controller
