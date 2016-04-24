#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>
#include <QProcess>
#include <QDate>

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
    void startCopy();
    QString getPath(QString settingString, QDate date, QString extension);

protected slots:
    void handleFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void copyFinished();

signals:
   void processStarted();
   void readyRead();
   void copyProgress(int progress);
   void copyError(QString error);
   void encodingFinished();

private:
   QProcess* ffmpegProcess;
   QString location1Path;
   QString location2Path;
   QString videoExtension;
   QString podcastExtension;
   bool secondConversion;
   bool secondCopy;
   bool canceled;
};

#endif // FILEHELPER_H
