#pragma once

#include <QObject>
#include <QThread>

namespace Config {
class AppConfig;
}

namespace Controller {
class Controller;
}

namespace Ui {
class MainWindow;
}

class Application : public QObject {
    Q_OBJECT

public:
    explicit Application(const Config::AppConfig& appConfig, QObject* parent = nullptr);
    ~Application() override;

    void start();

private:
    const Config::AppConfig& m_appConfig;
    QThread m_controllerThread;
    Controller::Controller* m_controller = nullptr;
    Ui::MainWindow* m_mainWindow = nullptr;
};
