#ifndef ASYNCIO_H
#define ASYNCIO_H

#include <QObject>

class AsyncIO : public QObject
{
    Q_OBJECT
public:
    explicit AsyncIO(QString existingFilePath, QString newFilePath, QObject *parent = 0);
    ~AsyncIO();

private:
    int progress;
    QString existingFilePath;
    QString newFilePath;

signals:
    void progressUpdate(int progress);
    void finished();
    void error(QString error);

public slots:
    void startCopy();
};

#endif // ASYNCIO_H
