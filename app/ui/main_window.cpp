#include "app/ui/main_window.h"

#include "app/config/modules/ui/ui_config.h"
#include "app/controller/device/device_operation_error.h"
#include "app/ui/device/device_presenter.h"
#include "app/ui/device/device_tree_model.h"
#include "app/ui/device/journal_model.h"
#include "app/ui/device/sticky_scroll_controller.h"

#include <QAbstractItemView>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QTableView>
#include <QTreeView>
#include <QVBoxLayout>
#include <QWidget>

namespace {

constexpr auto DeviceOperationErrorDisplayMs = 5000;
constexpr auto InitialWindowWidth = 1600;
constexpr auto InitialWindowHeight = 1200;

}  // namespace

namespace Ui {

MainWindow::MainWindow(const Config::Ui::UiConfig& config, QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("Device monitor"));
    resize(InitialWindowWidth, InitialWindowHeight);

    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout* actionLayout = new QHBoxLayout();

    m_refreshButton = new QPushButton(tr("Update"), centralWidget);
    actionLayout->addStretch();
    actionLayout->addWidget(m_refreshButton);

    m_splitter = new QSplitter(Qt::Horizontal, centralWidget);
    QGroupBox* devicePanel = new QGroupBox(tr("Current state"), m_splitter);
    QGroupBox* journalPanel = new QGroupBox(tr("Event journal"), m_splitter);
    QVBoxLayout* devicePanelLayout = new QVBoxLayout(devicePanel);
    QVBoxLayout* journalPanelLayout = new QVBoxLayout(journalPanel);
    m_deviceTree = new QTreeView(devicePanel);
    m_journal = new QTableView(journalPanel);
    devicePanelLayout->addWidget(m_deviceTree);
    journalPanelLayout->addWidget(m_journal);

    m_deviceTreeModel = new Device::DeviceTreeModel(this);
    m_journalModel = new Device::JournalModel(config, this);
    m_devicePresenter = new Device::DevicePresenter(m_deviceTreeModel, m_journalModel, this);
    m_stickyScrollController = new Device::StickyScrollController(m_journal, m_journalModel, this);

    m_deviceTree->setModel(m_deviceTreeModel);
    m_deviceTree->setAlternatingRowColors(true);
    m_deviceTree->setUniformRowHeights(true);
    m_deviceTree->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_deviceTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_deviceTree->header()->setSectionResizeMode(
        static_cast<int>(Device::DeviceTreeModel::Column::DeviceOrMetric),
        QHeaderView::Stretch);
    m_deviceTree->header()->setSectionResizeMode(
        static_cast<int>(Device::DeviceTreeModel::Column::Id),
        QHeaderView::ResizeToContents);
    m_deviceTree->header()->setSectionResizeMode(
        static_cast<int>(Device::DeviceTreeModel::Column::Value),
        QHeaderView::ResizeToContents);
    m_deviceTree->header()->setSectionResizeMode(
        static_cast<int>(Device::DeviceTreeModel::Column::State),
        QHeaderView::ResizeToContents);

    m_journal->setModel(m_journalModel);
    m_journal->setAlternatingRowColors(true);
    m_journal->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_journal->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_journal->setSortingEnabled(false);
    m_journal->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_journal->verticalHeader()->hide();
    m_journal->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_journal->horizontalHeader()->setSectionResizeMode(
        static_cast<int>(Device::JournalModel::Column::Message),
        QHeaderView::Stretch);

    m_splitter->addWidget(devicePanel);
    m_splitter->addWidget(journalPanel);
    m_splitter->setStretchFactor(0, 2);
    m_splitter->setStretchFactor(1, 3);

    mainLayout->addLayout(actionLayout);
    mainLayout->addWidget(m_splitter);
    setCentralWidget(centralWidget);
    statusBar()->showMessage(tr("Ready"));

    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::refreshRequested);
    connect(
        m_deviceTreeModel,
        &QAbstractItemModel::rowsInserted,
        this,
        [this](const QModelIndex& parent) {
            if (parent.isValid()) {
                m_deviceTree->expand(parent);
            }
        }
    );
}

void MainWindow::presentDevices(const Domain::DeviceList& devices)
{
    m_devicePresenter->presentDevices(devices);
}

void MainWindow::presentEvents(const Domain::DeviceEventList& events)
{
    m_devicePresenter->presentEvents(events);
}

void MainWindow::presentUnreliableDevices(const QStringList& deviceIds)
{
    m_devicePresenter->presentUnreliableDevices(deviceIds);
}

void MainWindow::presentDeviceOperationFailure(const Controller::Device::DeviceOperationError& error)
{
    statusBar()->showMessage(error.message, DeviceOperationErrorDisplayMs);
}

}  // namespace Ui
