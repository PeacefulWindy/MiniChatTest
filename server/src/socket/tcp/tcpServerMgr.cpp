#include "tcpServerMgr.hpp"
#include<socket/tcp/tcpServer.hpp>
#include<socket/session/tcpServerSession.hpp>
#include<msg/msg.hpp>
#include<queue/threadQueue.hpp>

extern ThreadQueue<std::shared_ptr<Msg>> GlobalMsgQueue;

TcpServerMgr* TcpServerMgr::mInst = nullptr;

TcpServerMgr* TcpServerMgr::getInstance()
{
	if (!TcpServerMgr::mInst)
	{
		TcpServerMgr::mInst = new TcpServerMgr();
	}

	return TcpServerMgr::mInst;
}

int32_t TcpServerMgr::newServer(std::string host, int32_t port, int32_t serviceId)
{
	this->mLock.lock();
	auto id = this->mAutoId;
	this->mAutoId++;
	auto tcpServer = std::make_shared<TcpServer>(id, host, port);
	if (serviceId > 0)
	{
		tcpServer->setServiceId(serviceId);
	}

	tcpServer->setOnAccept([=](TcpServerSession* session)
		{
			session->setOnRead([=](TcpServerSession* session, std::shared_ptr<std::vector<uint8_t>> data)
				{
					auto serviceId = tcpServer->getServiceId();
					if (serviceId <= 0)
					{
						return;
					}

					auto msg = std::make_shared<Msg>();
					msg->setProtocol(static_cast<int32_t>(MsgType::Socket));
					msg->setToServiceId(serviceId);
					msg->setCmd(std::to_string(id));
					msg->setSessionId(session->getFd());
					msg->setData(data);

					GlobalMsgQueue.lock();
					GlobalMsgQueue.push(msg);
					GlobalMsgQueue.unlock();
				});
		});

	this->mServers.insert({ id,tcpServer });
	this->mLock.unlock();

	return id;
}

void TcpServerMgr::destroyServer(int32_t id)
{
	this->mLock.lock();
	auto iter = this->mServers.find(id);
	if (iter == this->mServers.end())
	{
		this->mLock.unlock();
		return;
	}
	
	this->mServers.erase(iter);
	this->mLock.unlock();
}

void TcpServerMgr::send(int32_t id, int32_t fd,std::shared_ptr<std::vector<uint8_t>> data)
{
	this->mLock.lock();
	auto iter = this->mServers.find(id);
	if (iter == this->mServers.end())
	{
		this->mLock.unlock();
		return;
	}

	auto server = iter->second;
	this->mLock.unlock();
	server->send(fd, data);
}

void TcpServerMgr::closeFd(int32_t id,int32_t fd)
{
	this->mLock.lock();
	auto iter = this->mServers.find(id);
	if (iter == this->mServers.end())
	{
		this->mLock.unlock();
		return;
	}

	auto server = iter->second;
	this->mLock.unlock();

	server->closeFd(fd);
}

