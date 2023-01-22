#include "formmessage.h"
#include "ui_formmessage.h"

#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPalette>
#include <QDebug>

FormMessage::FormMessage(const QString &title, const QString &text, int sec, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormMessage)
{
    ui->setupUi(this);
    ui->captionLabel->setText(title);
    ui->messageLabel->setText(text);

    QPalette pal = ui->captionLabel->palette();
    pal.setColor(QPalette::Normal, QPalette::WindowText, QColor(253, 255,255));
    pal.setColor(QPalette::Inactive, QPalette::WindowText, QColor(Qt::white));
    ui->captionLabel->setPalette(pal);


    QPalette mpal = ui->messageLabel->palette();
    mpal.setColor( QPalette::Normal, QPalette::WindowText, QColor(Qt::white));
    mpal.setColor( QPalette::Inactive, QPalette::WindowText, QColor(Qt::white));
    ui->messageLabel->setPalette(mpal);



    setWindowFlags(Qt::WindowStaysOnTopHint | Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    if(sec > 0){
        timer = new QTimer();
        timer->setInterval(sec * 1000);
        timer->start();
        connect(timer, &QTimer::timeout, this, &FormMessage::timerout);
    }

    connect(ui->closeButton, &QPushButton::clicked, this, &FormMessage::closeButtonClicked);
}

FormMessage::~FormMessage()
{
    delete ui;
}

void FormMessage::paintEvent(QPaintEvent *event)
{
    //qDebug() << event->rect();
    QPainter painter(this);

    QPen pen = painter.pen();
    pen.setColor(Qt::cyan);
    pen.setWidthF(2.5);
    painter.setPen(pen);

    QBrush brush = painter.background();
    brush.setColor(QColor(61,60,58, 250));
    painter.setBackground(brush);


    painter.drawRect(0,0,rect().width(), rect().height());
    painter.fillRect(rect(), brush);

}

void FormMessage::timerout()
{
    closeButtonClicked();
}

void FormMessage::closeButtonClicked()
{
    if(timer->isActive())
        timer->deleteLater();

    close();
}
