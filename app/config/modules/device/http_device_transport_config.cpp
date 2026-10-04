#include "app/config/modules/device/http_device_transport_config.h"

#include <QSettings>

namespace {

constexpr auto BaseUrlKey = "DeviceApi/BaseUrl";
constexpr auto DefaultBaseUrl = "http://127.0.0.1:8080";

constexpr auto DevicesRequestTimeoutMsKey = "DeviceApi/DevicesRequestTimeoutMs";
constexpr auto DefaultDevicesRequestTimeoutMs = 5000;

constexpr auto PollRequestTimeoutMsKey = "DeviceApi/PollRequestTimeoutMs";
constexpr auto DefaultPollRequestTimeoutMs = 5000;

}  // namespace

namespace Config::Device {

HttpDeviceTransportConfig::HttpDeviceTransportConfig(const QSettings& settings)
    : m_baseUrl(settings.value(BaseUrlKey, DefaultBaseUrl).toUrl())
    , m_devicesRequestTimeoutMs(settings.value(DevicesRequestTimeoutMsKey, DefaultDevicesRequestTimeoutMs).toInt())
    , m_pollRequestTimeoutMs(settings.value(PollRequestTimeoutMsKey, DefaultPollRequestTimeoutMs).toInt())
{
    if (!m_baseUrl.isValid() || m_baseUrl.isEmpty()) {
        m_baseUrl = QUrl(DefaultBaseUrl);
    }

    if (m_devicesRequestTimeoutMs <= 0) {
        m_devicesRequestTimeoutMs = DefaultDevicesRequestTimeoutMs;
    }

    if (m_pollRequestTimeoutMs <= 0) {
        m_pollRequestTimeoutMs = DefaultPollRequestTimeoutMs;
    }
}

const QUrl& HttpDeviceTransportConfig::baseUrl() const
{
    return m_baseUrl;
}

int HttpDeviceTransportConfig::devicesRequestTimeoutMs() const
{
    return m_devicesRequestTimeoutMs;
}

int HttpDeviceTransportConfig::pollRequestTimeoutMs() const
{
    return m_pollRequestTimeoutMs;
}

}  // namespace Config::Device
