#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>
#include <QProcess>

class FileHelper : public QObject
{
    Q_OBJECT

public:
    FileHelper(QObject *parent = 0);
    ~FileHelper();

    Q_INVOKABLE bool fileExists(QString path);

public slots:
    void start();
    QString getOutput();
    void cancel();

protected:
    QStringList splitArgumentString(QString argString);
    void startSecondConversion();

protected slots:
    void handleFinish(int exitCode, QProcess::ExitStatus exitStatus);

signals:
   void processStarted();
   void readyRead();
   void encodingFinished();

private:
   QProcess* ffmpegProcess;
   QString location1Path;
   bool secondConversion;
};

#endif // FILEHELPER_H
