#include "app/controller/device/device_poller.h"

#include "app/config/modules/device/device_poller_config.h"

#include <QTimer>

namespace Controller::Device {

DevicePoller::DevicePoller(
    const Config::Device::DevicePollerConfig& config,
    QObject* parent)
    : QObject(parent)
    , m_config(config)
    , m_timer(new QTimer(this))
{
    m_timer->setInterval(m_config.intervalMs());
    connect(m_timer, &QTimer::timeout, this, &DevicePoller::pollRequested);
}

int DevicePoller::intervalMs() const
{
    return m_timer->interval();
}

bool DevicePoller::isActive() const
{
    return m_timer->isActive();
}

void DevicePoller::start()
{
    m_timer->start();
}

void DevicePoller::stop()
{
    m_timer->stop();
}

void DevicePoller::restart()
{
    m_timer->stop();
    m_timer->start();
}

void DevicePoller::setIntervalMs(int intervalMs)
{
    if (intervalMs <= 0) {
        return;
    }

    m_timer->setInterval(intervalMs);
}

}  // namespace Controller::Device
