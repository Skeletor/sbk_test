#include "app/controller/device/idevice_transport.h"

namespace Controller::Device {

IDeviceTransport::IDeviceTransport(QObject* parent)
    : QObject(parent)
{
}

IDeviceTransport::~IDeviceTransport() = default;

}  // namespace Controller::Device
