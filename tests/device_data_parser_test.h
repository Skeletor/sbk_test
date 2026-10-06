#pragma once

#include <QObject>

class DeviceDataParserTest : public QObject {
    Q_OBJECT

private slots:
    void parseDevices();
    void skipInvalidDevices();
    void rejectInvalidDevicesDocument_data();
    void rejectInvalidDevicesDocument();

    void parsePoll();
    void skipInvalidEvents();
    void rejectInvalidPollDocument_data();
    void rejectInvalidPollDocument();
};
