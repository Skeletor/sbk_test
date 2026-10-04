#pragma once

#include <QObject>

namespace Config {
class AppConfig;
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
    Ui::MainWindow* m_mainWindow = nullptr;
};
