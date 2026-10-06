#include "app/config/modules/device/device_transport_config.h"

#include <QLoggingCategory>
#include <QSettings>

namespace {

Q_LOGGING_CATEGORY(DeviceTransportConfigLog, "config.device.transport")

constexpr auto TransportKey = "DeviceApi/Transport";
constexpr auto HttpTransportName = "http";
constexpr auto MockTransportName = "mock";

}  // namespace

namespace Config::Device {

DeviceTransportConfig::DeviceTransportConfig(QSettings& settings)
{
    if (!settings.contains(TransportKey)) {
        settings.setValue(TransportKey, HttpTransportName);
    }

    const QString transportName = settings.value(TransportKey).toString().trimmed().toLower();
    if (transportName == HttpTransportName) {
        m_type = DeviceTransportType::Http;
    } else if (transportName == MockTransportName) {
        m_type = DeviceTransportType::Mock;
    } else {
        qCWarning(DeviceTransportConfigLog)
            << "Unknown device transport:" << transportName
            << ", using" << HttpTransportName;
        m_type = DeviceTransportType::Http;
    }
}

DeviceTransportType DeviceTransportConfig::type() const
{
    return m_type;
}

}  // namespace Config::Device
