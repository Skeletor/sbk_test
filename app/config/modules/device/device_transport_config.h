#pragma once

class QSettings;

namespace Config::Device {

enum class DeviceTransportType {
    Http,
    Mock,
};

class DeviceTransportConfig {
public:
    explicit DeviceTransportConfig(QSettings& settings);

    DeviceTransportType type() const;

private:
    DeviceTransportType m_type = DeviceTransportType::Http;
};

}  // namespace Config::Device
