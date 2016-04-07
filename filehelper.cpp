#include "filehelper.h"

#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>
#include <QCoreApplication>
#include <QDate>
#include <QStack>

FileHelper::FileHelper(QObject *parent)
    : QObject(parent), maxMultiFileTimeCutoff(7201)//2 hours & 1 second
{
}

FileHelper::~FileHelper()
{
    cancel();
}

bool FileHelper::fileExists(QString path)
{
    return QDir(path).exists();
}

//search recursively for older files with a numbered filename of one less than lastNum
QStack<QString> FileHelper::recursiveDateSearch(QStack<QString> resultStack, QString path, QString startsWith, int lastNum, QDateTime lastVideoTime)
{
    QDirIterator dirIt(path);
    QDateTime adjustedDateTime = lastVideoTime;
    adjustedDateTime.setTime(lastVideoTime.time().addSecs(-maxMultiFileTimeCutoff));

    QSettings settings;

    while(dirIt.hasNext())
    {
        dirIt.next();
        //need to get the biggest file
        if(dirIt.fileName().startsWith(startsWith) /*&& dirIt.fileInfo().created() >= adjustedDateTime*/ && dirIt.fileInfo().created()<lastVideoTime)
        {

            qDebug() << "Found possible file";
            QString tempString = dirIt.fileInfo().baseName().replace(settings.value("sourceName", "").toString(), "");
            int number = tempString.toDouble();
            qDebug() << number;
            if(number==(lastNum-1))
            {
                resultStack.push(dirIt.fileInfo().fileName());
                //try to continue searching in case there are more split files
                return recursiveDateSearch(resultStack, path, startsWith, lastNum-1, dirIt.fileInfo().created());
//                return dirIt.filePath() + "\n" + recursiveDateSearch(path, startsWith, lastNum-1, dirIt.fileInfo().created());
            }
        }
    }
    //final split file (or no split file) found, so return results
    return resultStack;
}

//start conversion
void FileHelper::start()
{
    secondConversion = false;
    QSettings settings;
    qDebug() << "starting";
    QString argString = settings.value("videoArg", "").toString();
    QString sourcePath = settings.value("sourcePath", "").toString();

    //File names in below directory look as follows {sourceName}0000.mov
    //large recordings are split into multiple files with an incremented number {sourceName}0001.mov
    //subsequent recordings (not split recordings) are also incremented
    QDirIterator dirIt(sourcePath);
    //biggest in this case means biggest file number
    QString biggestFileName;
    int biggestNumber=0;
    QDateTime biggestFileCreated;
    while(dirIt.hasNext())
    {
        dirIt.next();
        if(dirIt.fileName().startsWith(settings.value("sourceName", "").toString()))
        {
            qDebug() << "Found possible file";
            //the replace basically removes everything except the number
            QString tempString = dirIt.fileInfo().baseName().replace(settings.value("sourceName", "").toString(), "");
            //convert string number to integer
            int number = tempString.toDouble();
            qDebug() << number;
            if(number>biggestNumber)
            {
                biggestFileName = dirIt.fileName();
                biggestNumber = number;
                biggestFileCreated = dirIt.fileInfo().created();
            }
        }
    }
    QString finalSourcePath = sourcePath + "/" + biggestFileName;

    //write list of split files to myfiles.txt so that the "concat" function of ffmpeg will join them together
    //TODO change this to use a QPointer
    QStack<QString> resultStack;
    resultStack.push(biggestFileName);
    resultStack = recursiveDateSearch(resultStack, sourcePath, settings.value("sourceName", "").toString(), biggestNumber, biggestFileCreated);
    QFile fileList(sourcePath + "/mylist.txt");
    fileList.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&fileList);
    QString fileString;
    while(resultStack.count()>0)
    {
        out << "file '" << resultStack.pop() << "'";
        if(resultStack.count()!=0)
            out << "\n";
    }
    fileList.close();

    location1Path = settings.value("location1Path", "").toString() + "/" + settings.value("outputName", "").toString() + "_" + QDate::currentDate().toString("yyyy_MM_dd");
//    argString.replace("<source>", finalSourcePath).replace("<output>", location1Path);
    argString.replace("<source>", sourcePath + "/mylist.txt").replace("<output>", location1Path);
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

