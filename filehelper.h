#ifndef FILEHELPER_H
#define FILEHELPER_H

#include <QObject>

class FileHelper : public QObject
{
    Q_OBJECT

public:
    FileHelper(QObject *parent = 0);
    ~FileHelper();

    Q_INVOKABLE bool fileExists(QString path);
};

#endif // FILEHELPER_H
