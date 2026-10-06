#include "app/ui/device/device_presenter.h"

#include "app/ui/device/device_tree_model.h"
#include "app/ui/device/journal_model.h"

namespace Ui::Device {

DevicePresenter::DevicePresenter(
    DeviceTreeModel* deviceTreeModel,
    JournalModel* journalModel,
    QObject* parent)
    : QObject(parent)
    , m_deviceTreeModel(deviceTreeModel)
    , m_journalModel(journalModel)
{
}

void DevicePresenter::presentDevices(const Domain::DeviceList& devices)
{
    if (m_deviceTreeModel) {
        m_deviceTreeModel->setDevices(devices);
    }
}

void DevicePresenter::presentEvents(const Domain::DeviceEventList& events)
{
    if (m_deviceTreeModel) {
        m_deviceTreeModel->applyEvents(events);
    }

    if (m_journalModel) {
        m_journalModel->appendEvents(events);
    }
}

void DevicePresenter::presentUnreliableDevices(const QStringList& deviceIds)
{
    if (m_deviceTreeModel) {
        m_deviceTreeModel->setDevicesUnreliable(deviceIds);
    }
}

}  // namespace Ui::Device
