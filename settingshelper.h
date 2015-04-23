#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QObject>
#include <QSettings>

class SettingsHelper : public QSettings
{
    Q_OBJECT

public:
    SettingsHelper(QObject *parent = 0);
    ~SettingsHelper();

    Q_INVOKABLE QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    Q_INVOKABLE void setValue(const QString& key, const QVariant& value);
};

#endif // SETTINGSHELPER_H
