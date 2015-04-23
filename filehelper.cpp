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

