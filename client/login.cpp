#include "login.h"
#include "./ui_login.h"
#include<QRegularExpression>
#include<QMessageBox>
#include<QJsonDocument>
#include<QJsonObject>
#include "mainwindow.h"
#include"netmgr.h"

Login::Login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

#ifdef QT_DEBUG
    ui->lineEditIp->setText("127.0.0.1");
    ui->lineEditPort->setText("60000");
    ui->lineEditUser->setText("testUser");
    ui->lineEditPwd->setText("123456");
#endif

    auto netMgr=NetMgr::getInstance();
    netMgr->bindMsg(10000,std::bind(&Login::onLoginRet,this,std::placeholders::_1));
    netMgr->bindMsg(10001,std::bind(&Login::onLogoutRet,this,std::placeholders::_1));
    netMgr->bindMsg(10002,std::bind(&Login::onRegisterRet,this,std::placeholders::_1));
    connect(netMgr,&NetMgr::signalDisConnect,this,&Login::onLogout);
}

Login::~Login()
{
    delete ui;
}

void Login::onLoginRet(QJsonObject &data)
{
    if(!data.contains("code"))
    {
        return;
    }

    auto netMgr=NetMgr::getInstance();
    auto code=static_cast<LoginCode>(data["code"].toInt());
    if(code != LoginCode::Ok)
    {
        auto msgBox=QMessageBox(this);
        switch(code)
        {
        case LoginCode::Banner:
            msgBox.setText("被服务器拒绝登陆");
            break;
        case LoginCode::UserOrPwdErr:
            msgBox.setText("用户名或密码错误");
            break;
        default:
            msgBox.setText("未知错误");
            break;
        }
        msgBox.exec();
        netMgr->close();
        return;
    }

    this->mMainWindow=new MainWindow();
    this->mMainWindow->show();
    this->hide();
}

void Login::onLogoutRet(QJsonObject &data)
{
    if(this->mMainWindow)
    {
        delete this->mMainWindow;
        this->mMainWindow=nullptr;
    }

    this->show();
}

void Login::onRegisterRet(QJsonObject &data)
{
    if(!data.contains("code"))
    {
        return;
    }

    auto netMgr=NetMgr::getInstance();
    auto code=static_cast<RegisterCode>(data["code"].toInt());
    auto msgBox=QMessageBox(this);
    switch(code)
    {
    case RegisterCode::Ok:
        msgBox.setText("注册成功，请登陆");
        break;
    case RegisterCode::Exist:
        msgBox.setText("该用户已注册");
        break;
    default:
        msgBox.setText("未知错误");
        break;
    }
    msgBox.exec();

    netMgr->close();
}

void Login::onLogout()
{
    if(!this->mMainWindow)
    {
        return;
    }

    auto msgBox=QMessageBox(this);
    msgBox.setText("与服务器断开连接，请重新登陆");
    msgBox.exec();

    delete this->mMainWindow;
    this->mMainWindow=nullptr;

    this->show();
}

void Login::on_uLoginBtn_clicked()
{
    auto host=this->ui->lineEditIp->text();
    auto port=this->ui->lineEditPort->text().toInt();
    auto user=this->ui->lineEditUser->text();
    auto pwd=this->ui->lineEditPwd->text();

    if(!this->checkLoginInfo(host,port,user,pwd))
    {
        return;
    }

    auto netMgr=NetMgr::getInstance();
    if(!netMgr->connectHost(host,port))
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("连接服务器失败");
        msgBox.exec();
    }

    auto data=QJsonObject();
    data.insert("user",user);
    data.insert("pwd",pwd);

    auto req=QJsonObject();
    req.insert("msgId",10000);
    req.insert("data",data);

    netMgr->send(QJsonDocument(req).toJson());
}


void Login::on_uRegisterBtn_clicked()
{
    auto host=this->ui->lineEditIp->text();
    auto port=this->ui->lineEditPort->text().toInt();
    auto user=this->ui->lineEditUser->text();
    auto pwd=this->ui->lineEditPwd->text();

    if(!this->checkLoginInfo(host,port,user,pwd))
    {
        return;
    }

    auto netMgr=NetMgr::getInstance();
    if(!netMgr->connectHost(host,port))
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("连接服务器失败");
        msgBox.exec();
    }

    auto data=QJsonObject();
    data.insert("user",user);
    data.insert("pwd",pwd);

    auto req=QJsonObject();
    req.insert("msgId",10002);
    req.insert("data",data);

    netMgr->send(QJsonDocument(req).toJson());
}

bool Login::checkLoginInfo(QString &host, int port, QString &user, QString pwd)
{
    auto regex=QRegularExpression();
    regex.setPattern("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");

    auto match=regex.match(host);
    if(!match.hasMatch())
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("不正确的ipv4地址，请重新输入");
        msgBox.exec();
        return false;
    }

    if(port <= 0 || port > 65535)
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("不正确的端口号，请重新输入");
        msgBox.exec();
        return false;
    }

    if(user.isEmpty())
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("请输入用户名");
        msgBox.exec();
        return false;
    }

    if(pwd.isEmpty())
    {
        auto msgBox=QMessageBox(this);
        msgBox.setText("请输入密码");
        msgBox.exec();
        return false;
    }

    return true;
}

