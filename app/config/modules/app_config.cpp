#include "app/config/modules/app_config.h"

#include <QDir>
#include <QLoggingCategory>
#include <QStandardPaths>

namespace {

Q_LOGGING_CATEGORY(AppConfigLog, "config.app")

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
    , m_uiConfig(m_settings)
{
    m_settings.sync();
    if (m_settings.status() != QSettings::NoError) {
        qCWarning(AppConfigLog) << "Failed to write the configuration:" << m_settings.fileName();
    }
}

const Controller::ControllerConfig& AppConfig::controllerConfig() const
{
    return m_controllerConfig;
}

const Ui::UiConfig& AppConfig::uiConfig() const
{
    return m_uiConfig;
}

}  // namespace Config
