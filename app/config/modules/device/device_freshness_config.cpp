#include "app/config/modules/device/device_freshness_config.h"

#include <QSettings>

namespace {

constexpr auto UnreliableAfterSecKey = "DeviceFreshness/UnreliableAfterSec";
constexpr auto DefaultUnreliableAfterSec = 300;

}  // namespace

namespace Config::Device {

DeviceFreshnessConfig::DeviceFreshnessConfig(QSettings& settings)
{
    if (!settings.contains(UnreliableAfterSecKey)) {
        settings.setValue(UnreliableAfterSecKey, DefaultUnreliableAfterSec);
    }

    m_unreliableAfterSec = settings.value(UnreliableAfterSecKey).toInt();
    if (m_unreliableAfterSec <= 0) {
        m_unreliableAfterSec = DefaultUnreliableAfterSec;
    }
}

int DeviceFreshnessConfig::unreliableAfterSec() const
{
    return m_unreliableAfterSec;
}

}  // namespace Config::Device
