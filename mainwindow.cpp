#include "mainwindow.h"
#include "./ui_mainwindow.h"


#include <QSettings>
#include <QMessageBox>

#include <QMultiHashIterator>
#include <QDesktopServices>

#include <QDebug>

#include "hashdir.h"
#include "formmessage.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle( qApp->applicationName() + qApp->applicationVersion() );

    model = new QStandardItemModel();
    ui->listView->setModel(model);


    readSettings();


    connect(ui->closeButton, &QPushButton::clicked, this, &MainWindow::closeButtonClicked);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::saveButtonClicked);
    connect(ui->openButton, &QPushButton::clicked, this, &MainWindow::openButtonClicked);
    connect(ui->reloadHashButton , &QToolButton::clicked, this, &MainWindow::reloadButtonClicked);
    connect(ui->findButton, &QPushButton::clicked, this, &MainWindow::findButtonClicked);
    connect(ui->clearAllButton, &QPushButton::clicked, this, &MainWindow::clearAllButtonClicked);
    connect(ui->deleteItemButton, &QToolButton::clicked, this, &MainWindow::deleteItemButtonClicked);
    connect(ui->fileLineEdit , &QLineEdit::textChanged, this, &MainWindow::fileEditTextChanged);
    connect(ui->filtersLineEdit , &QLineEdit::textChanged, this, &MainWindow::filtersEditTextChanged);
    connect(ui->filePathEdit , &QLineEdit::textChanged, this, &MainWindow::filePathTextChanged);

    connect(ui->checkAllBox , &QCheckBox::stateChanged, this, &MainWindow::checkAllBoxClicked);
    connect(ui->useHashkBox , &QCheckBox::stateChanged, this, &MainWindow::useHashBoxClicked);

    connect(ui->listView, &QListView::clicked, this, &MainWindow::listViewClicked );


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{

    // When application is running a thread don't close the app
    // until the threads send signals finished
    if(pool->activeThreadCount() > 0){
        QMessageBox::information(this, tr("Information"), tr("This application is still running"
                                                             " in background, please wait for finished!"));

        event->ignore();
        return;
    }

    saveSettings();
    event->accept();
}

void MainWindow::closeButtonClicked()
{
    //saveSettings();
    close();
}

void MainWindow::saveButtonClicked()
{
    // Save the listView
    saveList();

}

void MainWindow::openButtonClicked()
{
    if(fileExist(ui->filePathEdit->text())){
        QDesktopServices::openUrl( QUrl::fromLocalFile(ui->filePathEdit->text())  );
    }

}

void MainWindow::findButtonClicked()
{

    // New search started clear results
    ui->currentPathEdit->clear();
    ui->filePathEdit->clear();

    if(useHash && checkAll){
        QString fn = ui->fileLineEdit->text();

        if(dirHash.contains(fn)){
            QStringList fList;
            QMultiHashIterator<QString, QString> it(dirHash);
            while (it.hasNext()) {

                auto i = it.next();

                QString f = i.key();
                QString ps = i.value();

                if(f == fn)
                    fList << ps;

            }

            foreach (QString s, fList) {

                QStandardItem *item = new QStandardItem(s);
                item->setEditable(false);
                model->appendRow( item );
            }


        }else
            showMessage( tr("File not found"), tr("DirHash not contains files: ") + fn, 5 );

        return;
    }

    if(useHash && !checkAll){
        QString fn = ui->fileLineEdit->text();
        if(dirHash.contains(fn)){
            QString p = dirHash.value(fn);
            ui->filePathEdit->setText(p);
            QStandardItem *item = new QStandardItem(p);
            item->setEditable(false);
            model->appendRow( item );
        }else
            showMessage( tr("File not found"), tr("DirHash not contains file: ") + fn, 5 );


        return;
    }

    if(ui->findButton->text() == "cancel"){
        worker->cancelWork();
        return;
    }


    // Startind to find file
    worker = new Worker( ui->fileLineEdit->text(), getFilters(), checkAll );

    // This signal emit when file mahtches filename
    QObject::connect(worker, &Worker::sendFilePath, this, &MainWindow::getFilePath);

    // This signal emit when worker is in current path
    QObject::connect(worker, &Worker::sendCurrentPath, this, &MainWindow::getCurrentPath);

    // This signal emit when worker is finished
    QObject::connect(worker, &Worker::finished, this, &MainWindow::workerFinished);


    pool = QThreadPool::globalInstance();
    pool->start(worker);

    ui->findButton->setText("cancel");
    ui->findButton->setEnabled(true);

}

void MainWindow::reloadButtonClicked()
{
    // Reload hash
    loadHashDirectorie();

}

void MainWindow::clearAllButtonClicked()
{
    model->clear();
}

void MainWindow::deleteItemButtonClicked()
{
    int row =  ui->listView->selectionModel()->currentIndex().row();

    QModelIndex index =  ui->listView->selectionModel()->currentIndex();
    model->data(index);
    ui->listView->model()->removeRow(row);
    ui->deleteItemButton->setEnabled(false);
}

void MainWindow::checkAllBoxClicked(int)
{
    checkAll = ui->checkAllBox->isChecked();
}

void MainWindow::useHashBoxClicked(int)
{
    useHash = ui->useHashkBox->isChecked();
}

void MainWindow::fileEditTextChanged(const QString &text)
{
    if(text.isEmpty())
        ui->findButton->setEnabled(false);
    else
        ui->findButton->setEnabled(true);
}

void MainWindow::filePathTextChanged(const QString &text)
{
    if(text.isEmpty())
        ui->openButton->setEnabled(false);
    else {
        if(fileExist(ui->filePathEdit->text()))
            ui->openButton->setEnabled(true);
    }
}

void MainWindow::filtersEditTextChanged(const QString &text)
{
    if(text.isEmpty())
        ui->reloadHashButton->setEnabled(false);
    else
        ui->reloadHashButton->setEnabled(true);
}

void MainWindow::listViewClicked(const QModelIndex &index)
{
    QVariant var = model->data(index);
    QString s = var.toString();
    ui->filePathEdit->setText(s);
    ui->deleteItemButton->setEnabled(true);
}


void MainWindow::getCurrentPath(const QString &path)
{
    ui->currentPathEdit->setText(path);
    ui->progressBar->setValue( ui->progressBar->value() + 1  );
}

// Signals from Worker emit when filename matches
void MainWindow::getFilePath(const QString &path)
{
    ui->filePathEdit->setText(path);

    QStandardItem *item = new QStandardItem(path);
    item->setEditable(false);

    model->appendRow(item);

    ui->itemsCounterBox->setValue( model->rowCount() );

}

// Worker is finished
void MainWindow::workerFinished()
{
    // Set find button
    ui->findButton->setText("&find");
    ui->progressBar->setValue(0);
    ui->itemsCounterBox->setValue(0);
    ui->currentPathEdit->clear();
}

// Signal from HashDir when loading finished
void MainWindow::hashDirFinished()
{
    ui->useHashkBox->setEnabled(true);
    ui->hashCounterBox->setValue( dirHash.size() );

    showMessage(tr("Loading hash"), tr("Loading hash successfull done!"), 5 );

}

// Signal from HashDir when loading finished
void MainWindow::getHashDir(const QMultiHash<QString, QString> &hash)
{
    dirHash = hash;
    ui->progressBar->setMaximum(dirHash.size());
}

// Signal from HashDir counter
void MainWindow::getHashDirCounter(int value)
{
    ui->hashCounterBox->setValue( value );
}

QStringList MainWindow::getFilters()
{
    QStringList filters;

    if(ui->filtersLineEdit->text().isEmpty())
        filters << "*.exe";
    else {

        QStringList fl = ui->filtersLineEdit->text().split(";");
        foreach (QString s, fl) {

            s.remove(" ");
            filters << s;
        }
    }

    return filters;
}

void MainWindow::loadHashDirectorie()
{

    QStringList filter = getFilters();
    HashDir *hashdir = new HashDir(filter);

    QObject::connect(hashdir, &HashDir::sendDirHash , this, &MainWindow::getHashDir);
    QObject::connect(hashdir, &HashDir::sendDirCounter  , this, &MainWindow::getHashDirCounter);
    QObject::connect(hashdir, &HashDir::finished , this, &MainWindow::hashDirFinished);

    pool = QThreadPool::globalInstance();
    pool->tryStart(hashdir);

}


void MainWindow::saveList()
{
    QStringList fList;
    for(int i = 0; i < model->rowCount(); i++)
        fList << model->index(i,0).data().toString();

    if(fList.isEmpty())
        return;

    QFile file("list.dat");
    if(file.open(QIODevice::WriteOnly)){

        QDataStream out(&file);
        out << fList;
    }
    else
        showMessage("Error saving: " , file.errorString(), 0 );

    file.close();
    ui->progressBar->setValue(0);
}

void MainWindow::loadList()
{
    QStringList fList;

    QFile file("list.dat");
    if(file.open(QIODevice::ReadOnly)){

        QDataStream in(&file);
        in >> fList;
    }
    else
        showMessage(tr("Loading Error"), file.errorString(), 0);

    file.close();


    if(!fList.isEmpty()){

        for (int i = 0; i <  fList.size(); i++) {
            QStandardItem *item = new QStandardItem(fList.at(i));
            item->setEditable(false);
            model->appendRow( item );
            if(!fileExist(fList.at(i)))
                    model->setData(model->index(i, 0), QBrush(QColor(255,0,0,50)), Qt::BackgroundRole);
        }

        showMessage(tr("Loading list"), tr("Loading the list successfull done!"), 5 );
    }
}

bool MainWindow::fileExist(const QString &filename)
{
    QFile file(filename);
    return file.exists();
}

void MainWindow::showMessage(const QString &title, const QString &message, int seconds)
{
    FormMessage *messagebox = new FormMessage(title, message, seconds, this);
    messagebox->setGeometry( geometry().x() +  this->width()/2 - messagebox->width()/2 , geometry().y() + this->height()/2, messagebox->width(), messagebox->height()  );
    //messagebox->setWindowFlags(Qt::Dialog);
    messagebox->show();
}


void MainWindow::readSettings()
{
    QSettings settings( qApp->organizationName(), qApp->applicationName());

    QRect rect = settings.value("geometrie", QRect(200,200,700,400) ).toRect();
    setGeometry(rect);

    checkAll = settings.value("checkall", false).toBool();
    ui->checkAllBox->setChecked(checkAll);

    progressMaxValue = 8500;
    ui->progressBar->setMaximum(progressMaxValue);
    ui->progressBar->setValue(0);

    loadHashDirectorie();
    loadList();

}

void MainWindow::saveSettings()
{
    QSettings settings( qApp->organizationName(), qApp->applicationName());
    settings.setValue("geometrie", this->geometry());
//    settings.setValue("usehash", ui->useHashkBox->isChecked());
    settings.setValue("checkall", ui->checkAllBox->isChecked());
}
































