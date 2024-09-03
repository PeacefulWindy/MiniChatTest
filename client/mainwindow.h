#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QJsonObject>
#include <QStandardItemModel>
#include <QWidget>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public:
    void init();

public:
    void onNewMsg(QJsonObject &data);

private slots:
    void on_btnSend_clicked();

    void on_btnClose_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
