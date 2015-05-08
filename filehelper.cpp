#include "filehelper.h"

#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>

FileHelper::FileHelper(QObject *parent)
    : QObject(parent)
{

}

FileHelper::~FileHelper()
{

}

bool FileHelper::fileExists(QString path)
{
    return QDir(path).exists();
}

void FileHelper::start()
{
    QSettings settings;
    qDebug() << "starting";
    QString argString = settings.value("videoArg", "").toString();
    QString sourcePath = settings.value("sourcePath", "").toString();
    QDirIterator dirIt(sourcePath);
    QString biggestFileName;
    int biggestNumber=0;
    while(dirIt.hasNext())
    {
        dirIt.next();
        if(dirIt.fileName().startsWith(settings.value("sourceName", "").toString()))
        {
            qDebug() << "Found possible file";
            QString tempString = dirIt.fileInfo().baseName().replace(settings.value("sourceName", "").toString(), "");\
            int number = tempString.toDouble();
            qDebug() << number;
            if(number>biggestNumber)
            {
                biggestFileName = dirIt.fileName();
                biggestNumber = number;
            }
        }
    }
    QString finalSourcePath = sourcePath + biggestFileName;
    qDebug() << finalSourcePath;
    QDir sourceDir(settings.value("sourcePath", "").toString());
    if(sourceDir.exists())
    {
//    ffmpegProcess = new QProcess(this);
//    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//    ffmpegProcess->setProcessEnvironment(env);
//    connect(ffmpegProcess, &QProcess::started, this, &FileHelper::processStarted);
//    connect(ffmpegProcess, &QProcess::readyReadStandardOutput, this, &FileHelper::readyReadStandardOutput);
//    connect(ffmpegProcess, &QProcess::readyReadStandardError, this, &FileHelper::readyReadStandardError);
//    connect(ffmpegProcess, (void (QProcess::*)(int,QProcess::ExitStatus))(&QProcess::finished), this, &FileHelper::handleFinish);
//    qDebug() << QDir::currentPath()+"ffmpeg.exe" << argString;
    //    ffmpegProcess.start(QDir::currentPath()+"ffmpeg.exe", argumentString);
    }
}

QString FileHelper::getStandardOutput()
{
    if(ffmpegProcess!=NULL)
        return ffmpegProcess->readAllStandardOutput();
    else
        return QString();
}

QString FileHelper::getStandardError()
{
    if(ffmpegProcess!=NULL)
        return ffmpegProcess->readAllStandardError();
    else
        return QString();
}

void FileHelper::cancel()
{
    if(ffmpegProcess!=NULL && ffmpegProcess->isOpen())
        ffmpegProcess->close();

    ffmpegProcess->deleteLater();
    ffmpegProcess = NULL;
}

void FileHelper::handleFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    ffmpegProcess->deleteLater();
    ffmpegProcess = NULL;
    emit encodingFinished();
}

