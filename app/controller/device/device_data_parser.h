#pragma once

#include "app/domain/device/device_types.h"

#include <QByteArray>

#include <optional>

namespace Controller::Device::DeviceDataParser {

std::optional<Domain::DeviceList> parseDevices(const QByteArray& body);
std::optional<Domain::DeviceEventList> parsePoll(const QByteArray& body);

}  // namespace Controller::Device::DeviceDataParser
