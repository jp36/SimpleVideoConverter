#include "filehelper.h"

#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>
#include <QCoreApplication>
#include <QDate>

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
    secondConversion = false;
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
    QString finalSourcePath = sourcePath + "/" + biggestFileName;
    qDebug() << finalSourcePath;
    location1Path = settings.value("location1Path", "").toString() + "/" + settings.value("outputName", "").toString() + "_" + QDate::currentDate().toString("yyyy_MM_dd");
    argString.replace("<source>", finalSourcePath).replace("<output>", location1Path);
//    qDebug() << argString;
    ffmpegProcess = new QProcess(this);
    ffmpegProcess->setProcessChannelMode(QProcess::MergedChannels);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    ffmpegProcess->setProcessEnvironment(env);
    connect(ffmpegProcess, &QProcess::started, this, &FileHelper::processStarted);
    connect(ffmpegProcess, &QProcess::readyReadStandardOutput, this, &FileHelper::readyRead);
    connect(ffmpegProcess, &QProcess::readyReadStandardError, this, &FileHelper::readyRead);
    connect(ffmpegProcess, (void (QProcess::*)(int,QProcess::ExitStatus))(&QProcess::finished), this, &FileHelper::handleFinish);
//    connect(ffmpegProcess, (void (QProcess::*)(QProcess::ProcessError))(&QProcess::error), this, &FileHelper::handleFinish);
    qDebug() << QFileInfo(QCoreApplication::applicationFilePath()).canonicalPath()+"/ffmpeg.exe " + argString;
//    qDebug() << QDir::currentPath()+"/ffmpeg.exe " + argString;
//    QStringList argList = argString.sp
//    qDebug() << splitArgumentString(argString);
//    ffmpegProcess->start(QFileInfo(QCoreApplication::applicationFilePath()).canonicalPath()+"/ffmpeg.exe", splitArgumentString(argString));
    ffmpegProcess->start(QFileInfo(QCoreApplication::applicationFilePath()).canonicalPath()+"/ffmpeg.exe " + argString);
}


void FileHelper::startSecondConversion()
{
    secondConversion = true;
    QSettings settings;
    qDebug() << "starting";
    QString argString = settings.value("podcastArg", "").toString();
    argString.replace("<input>", location1Path).replace("<output>", location1Path);
    //delete old
    ffmpegProcess->deleteLater();
    //start new
    ffmpegProcess = new QProcess(this);
    ffmpegProcess->setProcessChannelMode(QProcess::MergedChannels);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    ffmpegProcess->setProcessEnvironment(env);
    connect(ffmpegProcess, &QProcess::started, this, &FileHelper::processStarted);
    connect(ffmpegProcess, &QProcess::readyReadStandardOutput, this, &FileHelper::readyRead);
    connect(ffmpegProcess, &QProcess::readyReadStandardError, this, &FileHelper::readyRead);
    connect(ffmpegProcess, (void (QProcess::*)(int,QProcess::ExitStatus))(&QProcess::finished), this, &FileHelper::handleFinish);
    ffmpegProcess->start(QFileInfo(QCoreApplication::applicationFilePath()).canonicalPath()+"/ffmpeg.exe " + argString);
}

QString FileHelper::getOutput()
{
    QString retValue;
    while(ffmpegProcess->canReadLine())
    {
        retValue.append(QString::fromLocal8Bit(ffmpegProcess->readLine()));
    }
    //Really not sure why I have to do below, but otherwise I get retValues of ""
    if(retValue == "")
        retValue = ffmpegProcess->readAllStandardOutput();
    return retValue;
}

void FileHelper::cancel()
{
    if(ffmpegProcess!=NULL && ffmpegProcess->isOpen())
        ffmpegProcess->close();

    ffmpegProcess->deleteLater();
    ffmpegProcess = NULL;
}

QStringList FileHelper::splitArgumentString(QString argString)
{
    QStringList list;

    // Add str sections to list including Sep and clear space at begin/end
    for( int i = 0; i < argString.count( '-' ); ++i ){
        list << argString.section( '-', i, i, QString::SectionIncludeLeadingSep|QString::SectionSkipEmpty ).simplified();
    }
    return list;
}

void FileHelper::handleFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(secondConversion)
    {
        secondConversion = false;
        ffmpegProcess->deleteLater();
        ffmpegProcess = NULL;
        emit encodingFinished();
    }
    else
        startSecondConversion();
}

