#include "worker.h"

#include <QThread>

Worker::Worker(const QString &filen, const QStringList &filters, bool all , QObject *parent)
    : QObject{parent}
{
    cancel = false;
    filename = filen;
    filter = filters;
    checkAll = all;
    found = false;
}

Worker::~Worker()
{

}

void Worker::cancelWork()
{
    cancel = true;
}

void Worker::run()
{
    doWork();
}

void Worker::doWork()
{
    // Do expensive hard work
    QStringList drivers;
    QFileInfoList dl = QDir::drives();
    foreach (QFileInfo info, dl) {
        drivers << info.filePath();
    }

    foreach (QString path, drivers) {

        if(cancel) break; // Stop working when user clicked cancel

        QDirIterator it(path, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {

            QString filepath = it.next();
            emit sendCurrentPath(filepath);

            if(cancel) break;  // Stop working when user clicked cancel

            QString fn = filepath.split("/").last();
            if(fn == filename){
                emit sendFilePath(filepath);
                found = true;
                if(!checkAll) break;
            }

            if(!checkAll && found)  break;
        }

        if(!checkAll && found)  break;
    }

    emit finished();

}
