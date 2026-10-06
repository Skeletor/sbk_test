#pragma once

#include "app/domain/device/device_types.h"

#include <QDateTime>
#include <QHash>
#include <QObject>
#include <QStringList>

class QTimer;

namespace Config::Device {
class DeviceFreshnessConfig;
}

namespace Controller::Device {

class DeviceFreshnessSentinel : public QObject {
    Q_OBJECT

public:
    explicit DeviceFreshnessSentinel(const Config::Device::DeviceFreshnessConfig& config, QObject* parent = nullptr);

    void processEvents(const Domain::DeviceEventList& events);
    void clear();

signals:
    void devicesBecameUnreliable(const QStringList& deviceIds);

private:
    void handleExpirationTimeout();
    void scheduleExpirationCheck();

private:
    const Config::Device::DeviceFreshnessConfig& m_config;
    QHash<QString, QDateTime> m_lastActivityByDevice;
    QTimer* m_expirationTimer = nullptr;
};

}  // namespace Controller::Device
