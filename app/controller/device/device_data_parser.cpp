#include "app/controller/device/device_data_parser.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QLoggingCategory>
#include <QSet>
#include <QVariant>

namespace {

const auto DevicesKey = QStringLiteral("devices");
const auto EventsKey = QStringLiteral("events");
const auto IdKey = QStringLiteral("id");
const auto NameKey = QStringLiteral("name");
const auto SequenceKey = QStringLiteral("seq");
const auto TimestampKey = QStringLiteral("ts");
const auto DeviceKey = QStringLiteral("device");
const auto TypeKey = QStringLiteral("type");
const auto MessageKey = QStringLiteral("message");
const auto MetricKey = QStringLiteral("metric");
const auto ValueKey = QStringLiteral("value");

const auto OnlineType = QStringLiteral("online");
const auto OfflineType = QStringLiteral("offline");
const auto ValueType = QStringLiteral("value");
const auto TimestampFormat = QStringLiteral("HH:mm:ss");

}  // namespace

Q_LOGGING_CATEGORY(DeviceDataParserLog, "controller.device.data_parser")

namespace {

std::optional<QJsonObject> parseRoot(const QByteArray& body)
{
    QJsonParseError error;
    const QJsonDocument document = QJsonDocument::fromJson(body, &error);
    if (error.error != QJsonParseError::NoError) {
        qCWarning(DeviceDataParserLog) << "Invalid JSON:" << error.errorString();
        return std::nullopt;
    }

    if (!document.isObject()) {
        qCWarning(DeviceDataParserLog) << "JSON root is not an object:" << document;
        return std::nullopt;
    }

    return document.object();
}

bool readString(
    const QJsonObject& object,
    const QString& key,
    QString& value,
    bool allowEmpty = false)
{
    const QJsonValue jsonValue = object.value(key);
    if (!jsonValue.isString()) {
        return false;
    }

    value = jsonValue.toString();
    return allowEmpty || !value.isEmpty();
}

bool readSequence(const QJsonObject& object, qint64& sequence)
{
    const QJsonValue jsonValue = object.value(SequenceKey);
    if (!jsonValue.isDouble()) {
        return false;
    }

    bool ok = false;
    const qint64 convertedSequence = jsonValue.toVariant().toLongLong(&ok);
    if (!ok
        || convertedSequence <= 0
        || static_cast<double>(convertedSequence) != jsonValue.toDouble()) {
        return false;
    }

    sequence = convertedSequence;
    return true;
}

std::optional<Domain::DeviceEvent> parseEvent(const QJsonValue& jsonValue)
{
    if (!jsonValue.isObject()) {
        qCWarning(DeviceDataParserLog) << "Event is not an object:" << jsonValue;
        return std::nullopt;
    }

    const QJsonObject object = jsonValue.toObject();
    Domain::DeviceEvent event;
    QString timestamp;
    QString type;
    if (!readSequence(object, event.sequence)
        || !readString(object, TimestampKey, timestamp)
        || !readString(object, DeviceKey, event.deviceId)
        || !readString(object, TypeKey, type)
        || !readString(object, MessageKey, event.message, true)) {
        qCWarning(DeviceDataParserLog) <<  "Incorrect/malformed event:" << jsonValue;
        return std::nullopt;
    }

    event.timestamp = QTime::fromString(timestamp, TimestampFormat);
    if (!event.timestamp.isValid()) {
        qCWarning(DeviceDataParserLog) << "Invalid timestamp:" << timestamp;
        return std::nullopt;
    }

    if (type == OnlineType) {
        event.type = Domain::EventType::Online;
    } else if (type == OfflineType) {
        event.type = Domain::EventType::Offline;
    } else if (type == ValueType) {
        QString metric;
        const QJsonValue value = object.value(ValueKey);
        if (!readString(object, MetricKey, metric) || !value.isDouble()) {
            qCWarning(DeviceDataParserLog) << "Invalid value:" << value;
            return std::nullopt;
        }

        event.type = Domain::EventType::Value;
        event.metric = metric;
        event.value = value.toDouble();
    } else {
        qCWarning(DeviceDataParserLog) << "Unknown type:" << type;
        return std::nullopt;
    }

    return event;
}

}  // namespace

namespace Controller::Device::DeviceDataParser {

std::optional<Domain::DeviceList> parseDevices(const QByteArray& body)
{
    const std::optional<QJsonObject> root = parseRoot(body);
    if (!root) {
        return std::nullopt;
    }

    const QJsonValue devicesValue = root->value(DevicesKey);
    if (!devicesValue.isArray()) {
        qCWarning(DeviceDataParserLog) << "Devices response is not an array:" << devicesValue;
        return std::nullopt;
    }

    Domain::DeviceList devices;
    QSet<QString> knownDeviceIds;
    const QJsonArray jsonDevices = devicesValue.toArray();
    for (int index = 0; index < jsonDevices.size(); ++index) {
        const QJsonValue jsonDevice = jsonDevices.at(index);
        if (!jsonDevice.isObject()) {
            qCWarning(DeviceDataParserLog) << "Device is not an object:" << jsonDevice;
            continue;
        }

        Domain::DeviceInfo device;
        const QJsonObject object = jsonDevice.toObject();
        if (!readString(object, IdKey, device.id)
            || !readString(object, NameKey, device.name)) {
            qCWarning(DeviceDataParserLog) << "Invalid device data:" << object;
            continue;
        }

        if (knownDeviceIds.contains(device.id)) {
            qCWarning(DeviceDataParserLog) << "Duplicated device id detected:" << device.id;
            continue;
        }

        knownDeviceIds.insert(device.id);
        devices.append(device);
    }

    return devices;
}

std::optional<Domain::DeviceEventList> parsePoll(const QByteArray& body)
{
    const std::optional<QJsonObject> root = parseRoot(body);
    if (!root) {
        return std::nullopt;
    }

    const QJsonValue eventsValue = root->value(EventsKey);
    if (!eventsValue.isArray()) {
        qCWarning(DeviceDataParserLog) << "Poll response is not an array:" << eventsValue;
        return std::nullopt;
    }

    Domain::DeviceEventList events;
    const QJsonArray jsonEvents = eventsValue.toArray();
    for (int index = 0; index < jsonEvents.size(); ++index) {
        std::optional<Domain::DeviceEvent> event = parseEvent(jsonEvents.at(index));
        if (event) {
            events.append(std::move(*event));
        }
    }

    return events;
}

}  // namespace Controller::Device::DeviceDataParser
