#pragma once

class QSettings;

namespace Config::Device {

class DevicePollerConfig {
public:
    explicit DevicePollerConfig(QSettings& settings);

    int intervalMs() const;

private:
    int m_intervalMs = 0;
};

}  // namespace Config::Device
