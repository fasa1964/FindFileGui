#ifndef FORMMESSAGE_H
#define FORMMESSAGE_H

#include <QWidget>
#include <QTimer>
#include <QPaintEvent>

namespace Ui {
class FormMessage;
}

class FormMessage : public QWidget
{
    Q_OBJECT

public:
    explicit FormMessage(const QString &title, const QString &text, int sec, QWidget *parent = nullptr);
    ~FormMessage();


protected:
    virtual void paintEvent(QPaintEvent *event) override;

private slots:
    void timerout();
    void closeButtonClicked();

private:
    Ui::FormMessage *ui;


    QTimer *timer;

};

#endif // FORMMESSAGE_H
