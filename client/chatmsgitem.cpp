#include "chatmsgitem.h"
#include "ui_chatmsgitem.h"

#include <QDateTime>

ChatMsgItem::ChatMsgItem(QString user,QString msg,float time,QListWidgetItem * item,QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ChatMsgItem)
    , mItem(item)
{
    ui->setupUi(this);

    auto dateTime=QDateTime().fromSecsSinceEpoch(time).toString("yyyy-MM-dd hh:mm:ss");
    auto userAndTimeText=dateTime+"\t"+user;
    this->ui->labelUserAndTime->setText(userAndTimeText);
    this->ui->labelMsg->setText(msg);
}

ChatMsgItem::~ChatMsgItem()
{
    delete ui;
}
