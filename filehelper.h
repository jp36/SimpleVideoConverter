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
    void startSecondConversion();
    QStack<QString> recursiveDateSearch(QStack<QString> resultStack, QString path, QString startsWith, int lastNum, QDateTime lastVideoTime);

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
   const qlonglong maxMultiFileTimeCutoff;
};

#endif // FILEHELPER_H
