#include "tests/device_data_parser_test.h"

#include "app/controller/device/device_data_parser.h"

#include <QByteArray>
#include <QTest>
#include <QTime>

namespace {

constexpr auto DevicesResponse = R"json(
{
    "devices": [
        {"id": "dev-17", "name": "Room sensor"},
        {"id": "dev-42", "name": "Outside sensor"}
    ]
}
)json";

constexpr auto PollResponse = R"json(
{
    "lastSeq": 46,
    "events": [
        {
            "seq": 44,
            "ts": "12:01:24",
            "device": "dev-17",
            "type": "value",
            "metric": "temperature",
            "value": 23.5,
            "message": "temperature = 23.5"
        },
        {
            "seq": 45,
            "ts": "12:01:25",
            "device": "dev-17",
            "type": "online",
            "message": "connection restored"
        },
        {
            "seq": 46,
            "ts": "12:01:26",
            "device": "dev-42",
            "type": "offline",
            "message": "no data for 60 s"
        }
    ]
}
)json";

}  // namespace

void DeviceDataParserTest::parseDevices()
{
    const std::optional<Domain::DeviceList> devices =
        Controller::Device::DeviceDataParser::parseDevices(DevicesResponse);

    QVERIFY(devices);
    QCOMPARE(devices->size(), 2);
    QCOMPARE(devices->at(0).id, QStringLiteral("dev-17"));
    QCOMPARE(devices->at(0).name, QStringLiteral("Room sensor"));
    QCOMPARE(devices->at(1).id, QStringLiteral("dev-42"));
    QCOMPARE(devices->at(1).name, QStringLiteral("Outside sensor"));
}

void DeviceDataParserTest::skipInvalidDevices()
{
    const QByteArray body = R"json(
    {
        "devices": [
            {"id": "dev-17", "name": "Room sensor"},
            {"id": "dev-18"},
            {"id": "", "name": "Empty id"},
            {"id": "dev-17", "name": "Duplicate"},
            42,
            {"id": "dev-42", "name": "Outside sensor"}
        ]
    }
    )json";

    const std::optional<Domain::DeviceList> devices =
        Controller::Device::DeviceDataParser::parseDevices(body);

    QVERIFY(devices);
    QCOMPARE(devices->size(), 2);
    QCOMPARE(devices->at(0).id, QStringLiteral("dev-17"));
    QCOMPARE(devices->at(1).id, QStringLiteral("dev-42"));
}

void DeviceDataParserTest::rejectInvalidDevicesDocument_data()
{
    QTest::addColumn<QByteArray>("body");

    QTest::newRow("invalid-json") << QByteArray("{");
    QTest::newRow("array-root") << QByteArray("[]");
    QTest::newRow("missing-devices") << QByteArray("{}");
    QTest::newRow("devices-not-array") << QByteArray(R"json({"devices": {}})json");
}

void DeviceDataParserTest::rejectInvalidDevicesDocument()
{
    QFETCH(QByteArray, body);

    const std::optional<Domain::DeviceList> devices =
        Controller::Device::DeviceDataParser::parseDevices(body);

    QVERIFY(!devices);
}

void DeviceDataParserTest::parsePoll()
{
    const std::optional<Domain::DeviceEventList> events =
        Controller::Device::DeviceDataParser::parsePoll(PollResponse);

    QVERIFY(events);
    QCOMPARE(events->size(), 3);

    const Domain::DeviceEvent& valueEvent = events->at(0);
    QCOMPARE(valueEvent.sequence, qint64(44));
    QCOMPARE(valueEvent.timestamp, QTime(12, 1, 24));
    QCOMPARE(valueEvent.deviceId, QStringLiteral("dev-17"));
    QVERIFY(valueEvent.type == Domain::EventType::Value);
    QVERIFY(valueEvent.metric);
    QCOMPARE(*valueEvent.metric, QStringLiteral("temperature"));
    QVERIFY(valueEvent.value);
    QCOMPARE(*valueEvent.value, 23.5);
    QCOMPARE(valueEvent.message, QStringLiteral("temperature = 23.5"));

    const Domain::DeviceEvent& onlineEvent = events->at(1);
    QVERIFY(onlineEvent.type == Domain::EventType::Online);
    QVERIFY(!onlineEvent.metric);
    QVERIFY(!onlineEvent.value);

    const Domain::DeviceEvent& offlineEvent = events->at(2);
    QVERIFY(offlineEvent.type == Domain::EventType::Offline);
    QVERIFY(!offlineEvent.metric);
    QVERIFY(!offlineEvent.value);
}

void DeviceDataParserTest::skipInvalidEvents()
{
    const QByteArray body = R"json(
    {
        "lastSeq": 14,
        "events": [
            {"seq": 8, "ts": "12:01:24", "device": "dev-17", "type": "online", "message": "online"},
            {"seq": 8.5, "ts": "12:01:25", "device": "dev-17", "type": "online", "message": "fractional sequence"},
            {"seq": 9, "ts": "not-a-time", "device": "dev-17", "type": "offline", "message": "invalid time"},
            {"seq": 10, "ts": "12:01:27", "device": "dev-17", "type": "value", "value": 21.0, "message": "missing metric"},
            {"seq": 11, "ts": "12:01:28", "device": "dev-17", "type": "unknown", "message": "unknown type"},
            42,
            {"seq": -12, "ts": "12:01:29", "device": "dev-17", "type": "online", "message": "negative sequence"},
            {"seq": 14, "ts": "12:01:30", "device": "dev-42", "type": "offline", "message": "offline"}
        ]
    }
    )json";

    const std::optional<Domain::DeviceEventList> events =
        Controller::Device::DeviceDataParser::parsePoll(body);

    QVERIFY(events);
    QCOMPARE(events->size(), 2);
    QCOMPARE(events->at(0).sequence, qint64(8));
    QCOMPARE(events->at(1).sequence, qint64(14));
}

void DeviceDataParserTest::rejectInvalidPollDocument_data()
{
    QTest::addColumn<QByteArray>("body");

    QTest::newRow("invalid-json") << QByteArray("{");
    QTest::newRow("array-root") << QByteArray("[]");
    QTest::newRow("missing-events") << QByteArray("{}");
    QTest::newRow("events-not-array") << QByteArray(R"json({"events": {}})json");
}

void DeviceDataParserTest::rejectInvalidPollDocument()
{
    QFETCH(QByteArray, body);

    const std::optional<Domain::DeviceEventList> events =
        Controller::Device::DeviceDataParser::parsePoll(body);

    QVERIFY(!events);
}
