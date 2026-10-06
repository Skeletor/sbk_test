#pragma once

#include "app/controller/device/device_transport_types.h"

#include <QByteArray>

#include <optional>

namespace Controller::Device::DeviceDataParser {

std::optional<DeviceTopology> parseDevices(const QByteArray& body);
std::optional<DeviceEventBatch> parsePoll(const QByteArray& body);

}  // namespace Controller::Device::DeviceDataParser
