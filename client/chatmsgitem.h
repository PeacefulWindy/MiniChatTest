#ifndef CHATMSGITEM_H
#define CHATMSGITEM_H

#include <QListWidgetItem>
#include <QWidget>

namespace Ui {
class ChatMsgItem;
}

class ChatMsgItem : public QWidget
{
    Q_OBJECT

public:
    explicit ChatMsgItem(QString user,QString msg,float time,QListWidgetItem* item,QWidget *parent = nullptr);
    ~ChatMsgItem();

private:
    Ui::ChatMsgItem *ui;
    QListWidgetItem* mItem=nullptr;
};

#endif // CHATMSGITEM_H
