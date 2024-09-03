#include "chatmsgitem.h"
#include "mainwindow.h"
#include "netmgr.h"
#include "ui_mainwindow.h"

#include <QJsonDocument>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto netMgr=NetMgr::getInstance();

    netMgr->bindMsg(10010,std::bind(&MainWindow::onNewMsg,this,std::placeholders::_1));
}

MainWindow::~MainWindow()
{
    auto netMgr=NetMgr::getInstance();
    netMgr->unbindMsg(10010);

    delete ui;
}

void MainWindow::onNewMsg(QJsonObject &data)
{
    if(!data.contains("time") || !data.contains("user")|| !data.contains("msg"))
    {
        return;
    }

    auto user=data["user"].toString();
    auto time=static_cast<float>(data["time"].toDouble());
    auto msg=data["msg"].toString();
    auto pItem = new QListWidgetItem("");
    auto item=new ChatMsgItem(user,msg,time,pItem);

    auto size=item->size();
    pItem->setSizeHint(size);
    this->ui->listWidgetChat->addItem(pItem);
    this->ui->listWidgetChat->setItemWidget(pItem,item);
    this->ui->listWidgetChat->setCurrentItem(pItem);
}

void MainWindow::on_btnSend_clicked()
{
    QString text=this->ui->textEditChat->toPlainText();
    if(text.isEmpty())
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("请输入内容");
        msgBox.exec();
        return;
    }

    auto data=QJsonObject();
    data["msg"]=text;

    auto req=QJsonObject();
    req["msgId"]=10010;
    req["data"]=data;

    NetMgr::getInstance()->send(QJsonDocument(req).toJson());
}


void MainWindow::on_btnClose_clicked()
{
    auto req=QJsonObject();
    req["msgId"]=10001;
    req["data"]=QJsonObject();

    NetMgr::getInstance()->send(QJsonDocument(req).toJson());
}

