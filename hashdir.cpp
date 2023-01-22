#include "hashdir.h"

#include <QDir>
#include <QDirIterator>

HashDir::HashDir(const QStringList &filters, QObject *parent)
    : QObject{parent}
{
    filter = filters;
}

HashDir::~HashDir()
{

}

void HashDir::run()
{
    doWork();
}

void HashDir::doWork()
{
    QMultiHash<QString, QString> hash;
    QStringList drivers;
    QFileInfoList dl = QDir::drives();
    foreach (QFileInfo info, dl) {
        drivers << info.filePath();
    }

    foreach (QString path, drivers) {

        QDirIterator it(path, filter, QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
        while (it.hasNext()) {

            QString path = it.next();
            QString fn = path.split("/").last();

            hash.insert(fn, path);

            emit sendDirCounter( hash.count() );
        }
    }

    emit sendDirHash(hash);
    emit finished();
}
