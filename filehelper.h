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

    Q_INVOKABLE int platformId() const {
            int tempId = -1;
    #if defined(Q_OS_ANDROID)
            tempId = 0;
    #elif defined(Q_OS_IOS)
            tempId = 1;
    #elif defined(Q_OS_BLACKBERRY)
            tempId = 2;
    #elif defined(Q_OS_WINPHONE)
            tempId = 3;
    #elif defined(Q_OS_LINUX)
            tempId = 4;
    #elif defined(Q_OS_MAC)
            tempId = 5;
    #elif defined(Q_OS_WINRT)
            tempId = 7;
    #elif defined(Q_OS_WIN)
            tempId = 6;
    #elif defined(Q_OS_QNX)
            tempId = 8;
    #endif
            return tempId;
        }

public slots:
    void start();
    QString getOutput();
    void cancel();

protected:
    void startSecondConversion();
    QStack<QString> recursiveDateSearch(QStack<QString> resultStack, QString path, QString startsWith, int lastNum, QDateTime lastVideoTime);
    void startCopy();
    QString getPath(QString settingString, QDate date, QString extension);
    void doCleanupAndFinish();

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
   QString ffmpegString;
   QString ffmpegPath;
   QString listFileName;
   QList<QString> filePathList;
};

#endif // FILEHELPER_H
