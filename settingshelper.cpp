#include "settingshelper.h"

SettingsHelper::SettingsHelper(QObject *parent)
    : QSettings(parent)

{
}

SettingsHelper::~SettingsHelper()
{

}

QVariant SettingsHelper::value(const QString &key, const QVariant &defaultValue) const
{
    return QSettings::value(key, defaultValue);
}

void SettingsHelper::setValue(const QString &key, const QVariant &value)
{
    QSettings::setValue(key, value);
}

QString SettingsHelper::getVersion()
{
#ifdef APP_VERSION
    return APP_VERSION;
#else
    return "1.0";
#endif
}

