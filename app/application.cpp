#include "app/application.h"

#include "app/config/modules/app_config.h"
#include "app/controller/controller.h"

#include <QApplication>
#include <QMetaObject>

Application::Application(const Config::AppConfig& appConfig, QObject* parent)
    : QObject(parent)
    , m_appConfig(appConfig)
{
}

Application::~Application()
{
    if (m_controllerThread.isRunning()) {
        if (m_controller) {
            QMetaObject::invokeMethod(
                m_controller,
                [controller = m_controller]() {
                    controller->shutdown();
                },
                Qt::BlockingQueuedConnection);
        }

        m_controllerThread.quit();
        m_controllerThread.wait();
    }

    m_controller = nullptr;
    delete m_mainWindow;
}

void Application::start()
{
    if (m_controller || m_mainWindow) {
        return;
    }

    qRegisterMetaType<Domain::DeviceList>("Domain::DeviceList");
    qRegisterMetaType<Domain::DeviceEventList>("Domain::DeviceEventList");
    qRegisterMetaType<Controller::Device::TransportError>("Device::TransportError");

    m_controller = new Controller::Controller(m_appConfig.controllerConfig());
    m_controller->moveToThread(&m_controllerThread);

    connect(
        &m_controllerThread,
        &QThread::started,
        m_controller,
        &Controller::Controller::initialize);
    connect(
        &m_controllerThread,
        &QThread::finished,
        m_controller,
        &QObject::deleteLater);
    connect(
        qApp,
        &QCoreApplication::aboutToQuit,
        m_controller,
        &Controller::Controller::shutdown,
        Qt::BlockingQueuedConnection);
    connect(
        qApp,
        &QCoreApplication::aboutToQuit,
        this,
        [this]() {
            m_controllerThread.quit();
            m_controllerThread.wait();
            m_controller = nullptr;
        }
    );

    m_controllerThread.start();
}
