#pragma once

#include "app/config/modules/controller/controller_config.h"
#include "app/config/modules/ui/ui_config.h"

#include <QSettings>
#include <QString>

namespace Config {

class AppConfig {
public:
    AppConfig();
    explicit AppConfig(const QString& filePath);

    const Controller::ControllerConfig& controllerConfig() const;
    const Ui::UiConfig& uiConfig() const;

private:
    QSettings m_settings;
    Controller::ControllerConfig m_controllerConfig;
    Ui::UiConfig m_uiConfig;
};

}  // namespace Config
