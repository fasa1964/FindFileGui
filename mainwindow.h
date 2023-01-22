#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

#include <QThread>
#include <QThreadPool>
#include <QMultiHash>
#include <QStandardItemModel>

#include "worker.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();



protected:
    virtual void closeEvent(QCloseEvent *event);

private slots:
    void closeButtonClicked();
    void saveButtonClicked();
    void openButtonClicked();
    void findButtonClicked();
    void reloadButtonClicked();
    void clearAllButtonClicked();
    void deleteItemButtonClicked();

    void checkAllBoxClicked(int);
    void useHashBoxClicked(int);

    void fileEditTextChanged(const QString &text);
    void filePathTextChanged(const QString &text);
    void filtersEditTextChanged(const QString &text);
    void listViewClicked(const QModelIndex &index);

    // Signals from Worker
    void getCurrentPath(const QString &path);
    void getFilePath(const QString &path);
    void workerFinished();

    // Signal from HashDir
    void hashDirFinished();
    void getHashDir(const QMultiHash<QString, QString> &hash);
    void getHashDirCounter(int value);

private:
    Ui::MainWindow *ui;

    // Thread Worker
    QThreadPool *pool;          // Pointer of instance ThreadPool
    Worker *worker;             // Finding files in another thread

    bool checkAll;              // Find filenames that matches
    bool useHash;               // Use hash to find files or one file
    QStringList getFilters();

    // QListView
    QStandardItemModel *model;  // Model of QListView

    // Hash
    QMultiHash<QString, QString> dirHash;
    void loadHashDirectorie();

    int progressMaxValue;
    void saveList();
    void loadList();
    bool fileExist(const QString &filename);
    void showMessage(const QString &title, const QString &message, int seconds);

    // Read and save settings before closing or starting
    void readSettings();
    void saveSettings();

};
#endif // MAINWINDOW_H
