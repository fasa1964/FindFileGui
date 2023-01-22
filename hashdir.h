#ifndef HASHDIR_H
#define HASHDIR_H

#include <QObject>
#include <QRunnable>
#include <QMultiHash>

class HashDir : public QObject, public QRunnable
{
    Q_OBJECT
public:
    explicit HashDir(const QStringList &filters, QObject *parent = nullptr);
    ~HashDir();

signals:
    void finished();
    void sendDirCounter(int value);
    void sendDirHash(const QMultiHash<QString, QString> &hash);

    // QRunnable interface
public:
    void run() override;


private:
    void doWork();

    QStringList filter;

};

#endif // HASHDIR_H
