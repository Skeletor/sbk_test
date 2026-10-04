#pragma once

class QSettings;

namespace Config::Device {

class DeviceFreshnessConfig {
public:
    explicit DeviceFreshnessConfig(const QSettings& settings);

    int unreliableAfterSec() const;

private:
    int m_unreliableAfterSec = 0;
};

}  // namespace Config::Device
