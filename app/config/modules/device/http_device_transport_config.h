#pragma once

#include <QUrl>

class QSettings;

namespace Config::Device {

class HttpDeviceTransportConfig {
public:
    explicit HttpDeviceTransportConfig(QSettings& settings);

    const QUrl& baseUrl() const;
    int devicesRequestTimeoutMs() const;
    int pollRequestTimeoutMs() const;

private:
    QUrl m_baseUrl;
    int m_devicesRequestTimeoutMs = 0;
    int m_pollRequestTimeoutMs = 0;
};

}  // namespace Config::Device
