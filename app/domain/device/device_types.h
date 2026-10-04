#pragma once

#include <QMap>
#include <QMetaType>
#include <QString>
#include <QTime>
#include <QVector>

#include <optional>

namespace Domain {

enum class DeviceState {
    Unknown,
    Online,
    Offline,
    Unreliable,
};

enum class EventType {
    Value,
    Online,
    Offline,
};

struct DeviceInfo {
    QString id;
    QString name;
};

struct DeviceEvent {
    qint64 sequence = 0;
    QTime timestamp;
    QString deviceId;
    EventType type = EventType::Value;
    std::optional<QString> metric;
    std::optional<double> value;
    QString message;
};

struct DeviceSnapshot {
    DeviceInfo info;
    DeviceState state = DeviceState::Unknown;
    QMap<QString, double> metrics;
    QString lastMessage;
};

using DeviceList = QVector<DeviceInfo>;
using DeviceEventList = QVector<DeviceEvent>;

}  // namespace Domain

Q_DECLARE_METATYPE(Domain::DeviceState)
Q_DECLARE_METATYPE(Domain::EventType)
Q_DECLARE_METATYPE(Domain::DeviceInfo)
Q_DECLARE_METATYPE(Domain::DeviceEvent)
Q_DECLARE_METATYPE(Domain::DeviceSnapshot)
Q_DECLARE_METATYPE(Domain::DeviceList)
Q_DECLARE_METATYPE(Domain::DeviceEventList)
