#pragma once

#include "app/domain/device/device_types.h"

#include <QMainWindow>
#include <QStringList>

class QPushButton;
class QSplitter;
class QTableView;
class QTreeView;

namespace Config::Ui {
class UiConfig;
}

namespace Controller::Device {
struct DeviceOperationError;
}

namespace Ui::Device {
class DeviceTreeModel;
class DevicePresenter;
class JournalModel;
class StickyScrollController;
}

namespace Ui {

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(const Config::Ui::UiConfig& config, QWidget* parent = nullptr);

    void presentDevices(const Domain::DeviceList& devices);
    void presentEvents(const Domain::DeviceEventList& events);
    void presentUnreliableDevices(const QStringList& deviceIds);
    void presentDeviceOperationFailure(const Controller::Device::DeviceOperationError& error);

signals:
    void refreshRequested();

private:
    QSplitter* m_splitter = nullptr;
    QTreeView* m_deviceTree = nullptr;
    QTableView* m_journal = nullptr;
    QPushButton* m_refreshButton = nullptr;
    Device::DeviceTreeModel* m_deviceTreeModel = nullptr;
    Device::JournalModel* m_journalModel = nullptr;
    Device::DevicePresenter* m_devicePresenter = nullptr;
    Device::StickyScrollController* m_stickyScrollController = nullptr;
};

}  // namespace Ui
