#ifndef LOGIN_H
#define LOGIN_H

#include "mainwindow.h"

#include <QDialog>
#include<QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

enum class LoginCode
{
    Ok=1,
    UserOrPwdErr=2,
    Banner=3,
};

enum class LogoutCode
{
    Ok=1,
};

enum class RegisterCode
{
    Ok=1,
    Exist=2,
};

class Login : public QDialog
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

public:
    void onLoginRet(QJsonObject &data);
    void onLogoutRet(QJsonObject &data);
    void onRegisterRet(QJsonObject &data);

private slots:
    void on_uLoginBtn_clicked();
    void on_uRegisterBtn_clicked();
    void onLogout();

private:
    bool checkLoginInfo(QString &host,int port,QString &user,QString pwd);

    MainWindow * mMainWindow=nullptr;
    Ui::Login *ui;
};
#endif // LOGIN_H
