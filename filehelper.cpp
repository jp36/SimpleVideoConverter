#include "filehelper.h"

#include <QDir>
#include <QUrl>
#include <QDebug>
#include <QSettings>
#include <QDirIterator>
#include <QCoreApplication>
#include <QDate>
#include <QStack>
#include <QThread>

#include "asyncio.h"

FileHelper::FileHelper(QObject *parent)
    : QObject(parent), secondCopy(false), maxMultiFileTimeCutoff(7201)//2 hours & 1 second
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

void FileHelper::startCopy()
{
    QSettings settings;
    QString extension;
    if(secondCopy)
        extension = settings.value("podcastArg").toString().split("<output>")[1];
    else
        extension = settings.value("videoArg").toString().split("<output>")[1];
    //spawn thread
    QThread* thread = new QThread;
    //location1 = existing
    //location2 = copy destination
    AsyncIO* worker = new AsyncIO(location1Path + extension, location2Path + extension);
    worker->moveToThread(thread);

    //handle filehelper signal connections
    connect(worker, &AsyncIO::progressUpdate, this, &FileHelper::copyProgress);
    connect(worker, &AsyncIO::error, this, &FileHelper::copyError);
    connect(worker, &AsyncIO::finished, this, &FileHelper::copyFinished);
    //thread start starts copy once thread strated
    connect(thread, &QThread::started, worker, &AsyncIO::startCopy);
    //when finished recieved tell the thread to quit and then tell worker
    //and thread to delete themselves after finished
    connect(worker, &AsyncIO::finished, thread, &QThread::quit);
    connect(worker, &AsyncIO::finished, worker, &AsyncIO::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    thread->start();
}

QString FileHelper::getPath(QString settingString, QDate date)
{
    QSettings settings;
    QString locationSettingString = settings.value(settingString, "").toString();
    if(""!=locationSettingString)
        return "\"" + locationSettingString + "/" + settings.value("outputName", "").toString() + "_" + date.toString("yyyy_MM_dd") + "\"";
    else
        return locationSettingString;//return nothing
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
    int biggestNumber=-1;
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
//    QString finalSourcePath = sourcePath + "/" + biggestFileName;

    //write list of split files to myfiles.txt so that the "concat" function of ffmpeg will join them together
    //TODO change this to use a QPointer
    QStack<QString> resultStack;
    resultStack.push(biggestFileName);
    resultStack = recursiveDateSearch(resultStack, sourcePath, settings.value("sourceName", "").toString(), biggestNumber, biggestFileCreated);
    QFile fileList(sourcePath + "/mylist.txt");
    fileList.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
    QTextStream out(&fileList);
    while(resultStack.count()>0)
    {
        out << "file '" << resultStack.pop() << "'";
        if(resultStack.count()!=0)
            out << "\n";
    }
    fileList.close();

    //the escaped quotes are necessary for ffmpeg to work with paths with spaces
    QDate currentDate = QDate::currentDate();
    location1Path = getPath("location1Path", currentDate);
    location2Path = getPath("location2Path", currentDate);
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
        //start copy if location2path has been defined
        if(""==location2Path)
            emit encodingFinished();
        else
            startCopy();
    }
    else
        startSecondConversion();
}

void FileHelper::copyFinished()
{
    if(secondCopy)
        emit encodingFinished();
    else
    {
        secondCopy = true;
        startCopy();
    }
}

