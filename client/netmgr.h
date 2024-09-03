#ifndef NETMGR_H
#define NETMGR_H

#include<QObject>
#include<QJsonObject>
#include<QtNetwork/QTcpSocket>

class NetMgr : public QObject
{
    Q_OBJECT

public:
    static NetMgr* getInstance();

public:
    bool isConnect();
    bool connectHost(QString host, quint16 port);
    void send(QString data);
    void close();

public:
    void bindMsg(uint32_t msgId,std::function<void(QJsonObject&)> func);
    void unbindMsg(uint32_t msgId);

private:
    static NetMgr* mInst;

private:
    explicit NetMgr(QObject *parent = nullptr);
    virtual ~NetMgr();

private:
    QTcpSocket * mSocket=nullptr;
    QMap<uint32_t,std::function<void(QJsonObject &)>> mMsgCalls;

signals:
    void signalDisConnect();

private slots:
    void onConnect();
    void onDisConnect();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError err);

signals:
};

#endif // NETMGR_H
