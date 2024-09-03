#include "netmgr.h"
#include<QJsonDocument>

NetMgr* NetMgr::mInst=nullptr;

NetMgr *NetMgr::getInstance()
{
    if(!NetMgr::mInst)
    {
        NetMgr::mInst=new NetMgr();
    }

    return NetMgr::mInst;
}

NetMgr::NetMgr(QObject *parent)
    : QObject{parent}
{
    this->mSocket=new QTcpSocket(this);
    connect(this->mSocket,&QTcpSocket::connected,this,&NetMgr::onConnect);
    connect(this->mSocket,&QTcpSocket::errorOccurred,this,&NetMgr::onError);
    connect(this->mSocket,&QTcpSocket::disconnected,this,&NetMgr::onDisConnect);
    connect(this->mSocket,&QTcpSocket::readyRead,this,&NetMgr::onReadyRead);
}

NetMgr::~NetMgr()
{

}

void NetMgr::onConnect()
{
    qDebug("Connect ok!");
}

void NetMgr::onDisConnect()
{
    this->mSocket->close();
    emit signalDisConnect();
}

bool NetMgr::isConnect()
{
    return this->mSocket->isOpen();
}

bool NetMgr::connectHost(QString host,quint16 port)
{
    if(this->mSocket->isOpen())
    {
        return true;
    }

    this->mSocket->connectToHost(host,port);
    if(!this->mSocket->waitForConnected())
    {
        return false;
    }
    else
    {
        return true;
    }
}

void NetMgr::send(QString data)
{
    if(this->mSocket->state() != QTcpSocket::ConnectedState)
    {
        return;
    }

    this->mSocket->write(data.toLocal8Bit());
}

void NetMgr::close()
{
    this->mSocket->close();
}

void NetMgr::bindMsg(uint32_t msgId, std::function<void (QJsonObject &)> func)
{
    this->mMsgCalls.insert(msgId,func);
}

void NetMgr::unbindMsg(uint32_t msgId)
{
    this->mMsgCalls.remove(msgId);
}

void NetMgr::onReadyRead()
{
    auto rawData=this->mSocket->readAll();
    auto jsonData=QString::fromLocal8Bit(rawData.data());
    auto doc=QJsonDocument::fromJson(jsonData.toUtf8());
    auto root=doc.object();
    if(!root.contains("msgId"))
    {
        qDebug()<<"Invalid data!"<<rawData;
        return;
    }

    auto msgId=root["msgId"].toInt();
    auto iter=this->mMsgCalls.find(msgId);
    if(iter == this->mMsgCalls.end())
    {
        return;
    }

    auto data=QJsonObject();
    if(root.contains("data"))
    {
        data=root["data"].toObject();
    }

    iter.value()(data);
}

void NetMgr::onError(QAbstractSocket::SocketError err)
{
    switch(err)
    {
    case QAbstractSocket::SocketError::ConnectionRefusedError:
        this->mSocket->close();
        break;
    default:
        break;
    }
}
