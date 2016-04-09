#include "asyncio.h"

#include <QFile>

AsyncIO::AsyncIO(QString inExistingFilePath, QString inNewFilePath, QObject *parent)
    : QObject(parent)
{
    existingFilePath = inExistingFilePath.replace("\"", "");
    newFilePath = inNewFilePath.replace("\"", "");
}

AsyncIO::~AsyncIO()
{

}

void AsyncIO::startCopy()
{
    //same file, no need to copy
    if(existingFilePath.compare(newFilePath) == 0)
    {
        emit finished();
        return;
    }

    //load both files
    QFile existingFile(existingFilePath);
    QFile newFile(newFilePath);
    bool openExisting = existingFile.open( QIODevice::ReadOnly );
    bool openNew = newFile.open( QIODevice::WriteOnly );

    //if either file fails to open exit
    if(!openExisting || !openNew) {
        emit error("Failed to open file");
        return;
    }

    double existingFileSize = existingFile.size();
    double bytesCopied = 0;

    //copy contents
    uint BUFFER_SIZE = 16000;
    char* buffer = new char[BUFFER_SIZE];
    while(!existingFile.atEnd())
    {
        qint64 len = existingFile.read( buffer, BUFFER_SIZE );
        newFile.write( buffer, len );
        bytesCopied += BUFFER_SIZE;

        int copyProgress = (bytesCopied / existingFileSize) * 100;
        if(copyProgress!=progress)
        {
            progress = copyProgress;
            emit progressUpdate(progress);
        }
    }

    //deallocate buffer
    delete[] buffer;
    buffer = NULL;
    emit finished();
    return;
}

