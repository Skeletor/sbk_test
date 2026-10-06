#include "app/controller/device/device_freshness_sentinel.h"

#include "app/config/modules/device/device_freshness_config.h"

#include <QTimer>

#include <algorithm>
#include <limits>

namespace Controller::Device {

DeviceFreshnessSentinel::DeviceFreshnessSentinel(
    const Config::Device::DeviceFreshnessConfig& config,
    QObject* parent)
    : QObject(parent)
    , m_config(config)
    , m_expirationTimer(new QTimer(this))
{
    m_expirationTimer->setSingleShot(true);
    connect(
        m_expirationTimer,
        &QTimer::timeout,
        this,
        &DeviceFreshnessSentinel::handleExpirationTimeout);
}

void DeviceFreshnessSentinel::processEvents(const Domain::DeviceEventList& events)
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (const Domain::DeviceEvent& event : events) {
        if (event.type == Domain::EventType::Offline) {
            m_lastActivityByDevice.remove(event.deviceId);
        } else {
            m_lastActivityByDevice.insert(event.deviceId, now);
        }
    }

    scheduleExpirationCheck();
}

void DeviceFreshnessSentinel::clear()
{
    m_lastActivityByDevice.clear();
    m_expirationTimer->stop();
}

void DeviceFreshnessSentinel::handleExpirationTimeout()
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    const int unreliableAfterSec = m_config.unreliableAfterSec();
    QStringList expiredDeviceIds;

    auto it = m_lastActivityByDevice.begin();
    while (it != m_lastActivityByDevice.end()) {
        if (it.value().addSecs(unreliableAfterSec) <= now) {
            expiredDeviceIds.append(it.key());
            it = m_lastActivityByDevice.erase(it);
        } else {
            ++it;
        }
    }

    if (!expiredDeviceIds.isEmpty()) {
        emit devicesBecameUnreliable(expiredDeviceIds);
    }

    scheduleExpirationCheck();
}

void DeviceFreshnessSentinel::scheduleExpirationCheck()
{
    m_expirationTimer->stop();
    if (m_lastActivityByDevice.isEmpty()) {
        return;
    }

    const int unreliableAfterSec = m_config.unreliableAfterSec();
    QDateTime earliestExpiration;
    for (auto it = m_lastActivityByDevice.cbegin(); it != m_lastActivityByDevice.cend(); ++it) {
        const QDateTime expiration = it.value().addSecs(unreliableAfterSec);
        if (!earliestExpiration.isValid() || expiration < earliestExpiration) {
            earliestExpiration = expiration;
        }
    }

    const qint64 remainingMs = QDateTime::currentDateTimeUtc().msecsTo(earliestExpiration);
    const int timerInterval = static_cast<int>(std::min<qint64>(
        remainingMs,
        std::numeric_limits<int>::max()));
    m_expirationTimer->start(timerInterval);
}

}  // namespace Controller::Device
