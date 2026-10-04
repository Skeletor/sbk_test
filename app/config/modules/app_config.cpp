#include "app/config/modules/app_config.h"

#include <QDir>
#include <QStandardPaths>

namespace {

constexpr auto ConfigFileName = "config.ini";

QString defaultConfigPath()
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)).filePath(ConfigFileName);
}

}  // namespace

namespace Config {

AppConfig::AppConfig()
    : AppConfig(defaultConfigPath())
{
}

AppConfig::AppConfig(const QString& filePath)
    : m_settings(filePath, QSettings::IniFormat)
    , m_controllerConfig(m_settings)
{
}

const Controller::ControllerConfig& AppConfig::controllerConfig() const
{
    return m_controllerConfig;
}

}  // namespace Config
