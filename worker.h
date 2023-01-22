#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QRunnable>

#include <QDir>
#include <QDirIterator>


class Worker : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit Worker(const QString &filen /*filename*/,
                    const QStringList &filters /*filters e.g. *.png*/,
                    bool all  /* check all */, QObject *parent = nullptr);
    ~Worker();

    void cancelWork();


signals:
    void sendCurrentPath(const QString &path);
    void sendFilePath(const QString &path);
    void finished();

    // QRunnable interface
public:
    void run() override;



private:
    void doWork();

    bool cancel;
    bool checkAll;
    bool found;
    QStringList filter;
    QString filename;
};

#endif // WORKER_H
