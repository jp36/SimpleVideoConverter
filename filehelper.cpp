#include "filehelper.h"

#include <QDir>
#include <QUrl>
#include <QDebug>

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

void FileHelper::start(QString argumentString)
{
    ffmpegProcess = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    ffmpegProcess->setProcessEnvironment(env);
    connect(ffmpegProcess, &QProcess::started, this, &FileHelper::processStarted);
    connect(ffmpegProcess, &QProcess::readyReadStandardOutput, this, &FileHelper::readyReadStandardOutput);
    connect(ffmpegProcess, &QProcess::readyReadStandardError, this, &FileHelper::readyReadStandardError);
    connect(ffmpegProcess, (void (QProcess::*)(int,QProcess::ExitStatus))(&QProcess::finished), this, &FileHelper::handleFinish);
    qDebug() << QDir::currentPath()+"ffmpeg.exe" << argumentString;
    //    ffmpegProcess.start(QDir::currentPath()+"ffmpeg.exe", argumentString);
}

QString FileHelper::getStandardOutput()
{
    return ffmpegProcess->readAllStandardOutput();
}

QString FileHelper::getStandardError()
{
    return ffmpegProcess->readAllStandardError();
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

