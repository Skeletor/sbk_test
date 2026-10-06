#pragma once

#include "app/domain/device/device_types.h"

#include <QObject>
#include <QPointer>
#include <QStringList>

namespace Ui::Device {

class DeviceTreeModel;
class JournalModel;

class DevicePresenter : public QObject {
    Q_OBJECT

public:
    DevicePresenter(DeviceTreeModel* deviceTreeModel, JournalModel* journalModel, QObject* parent = nullptr);

    void presentDevices(const Domain::DeviceList& devices);
    void presentEvents(const Domain::DeviceEventList& events);
    void presentUnreliableDevices(const QStringList& deviceIds);

private:
    QPointer<DeviceTreeModel> m_deviceTreeModel;
    QPointer<JournalModel> m_journalModel;
};

}  // namespace Ui::Device
