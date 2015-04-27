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
    void start(QString argumentString);
    QString getStandardOutput();
    QString getStandardError();
    void cancel();

protected slots:
    void handleFinish(int exitCode, QProcess::ExitStatus exitStatus);

signals:
   void processStarted();
   void readyReadStandardOutput();
   void readyReadStandardError();
   void encodingFinished();

private:
   QProcess* ffmpegProcess;
};

#endif // FILEHELPER_H
