#include "app/config/modules/device/device_poller_config.h"

#include <QSettings>

namespace {

constexpr auto IntervalMsKey = "DevicePolling/IntervalMs";
constexpr auto DefaultIntervalMs = 2000;

}  // namespace

namespace Config::Device {

DevicePollerConfig::DevicePollerConfig(const QSettings& settings)
    : m_intervalMs(settings.value(IntervalMsKey, DefaultIntervalMs).toInt())
{
    if (m_intervalMs <= 0) {
        m_intervalMs = DefaultIntervalMs;
    }
}

int DevicePollerConfig::intervalMs() const
{
    return m_intervalMs;
}

}  // namespace Config::Device
