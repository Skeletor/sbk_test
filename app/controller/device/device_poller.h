#pragma once

#include <QObject>

class QTimer;

namespace Config::Device {
class DevicePollerConfig;
}

namespace Controller::Device {

class DevicePoller : public QObject {
    Q_OBJECT

public:
    explicit DevicePoller(const Config::Device::DevicePollerConfig& config, QObject* parent = nullptr);

    int intervalMs() const;
    bool isActive() const;

    void start();
    void stop();
    void restart();
    void setIntervalMs(int intervalMs);

signals:
    void pollRequested();

private:
    const Config::Device::DevicePollerConfig& m_config;
    QTimer* m_timer = nullptr;
};

}  // namespace Controller::Device
