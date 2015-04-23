#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>

#include "settingshelper.h"
#include "filehelper.h"

const char* uri = "org.crossway.converter";

QObject *settingsHelper_provider(QQmlEngine *engine, QJSEngine * scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new SettingsHelper(engine);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("Crossway");
    app.setApplicationName("converter");
    app.setOrganizationDomain("org.crossway");

//    qmlRegisterSingletonType<SettingsHelper>(uri, 1, 0, "SettingsHelper", settingsHelper_provider);

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("SettingsHelper", new SettingsHelper());
    engine.rootContext()->setContextProperty("FileHelper", new FileHelper());

    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
