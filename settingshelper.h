#ifndef SETTINGSHELPER_H
#define SETTINGSHELPER_H

#include <QObject>
#include <QSettings>

//Settings helper was needed originally as there was no way to
//access settings directly from QML
//could be replaced with http://doc.qt.io/qt-5/qml-qt-labs-settings-settings.html
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
