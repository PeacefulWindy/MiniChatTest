#include "tcpServer.hpp"
#include<hv/hloop.h>
#include<socket/session/tcpServerSession.hpp>
#include<spdlog/spdlog.h>
#include <hv/hsocket.h>

extern hloop_t* GlobalLoop;

TcpServer::TcpServer(int32_t id, std::string& host, int port)
	:mId(id)
{
	auto io = hloop_create_tcp_server(GlobalLoop, host.c_str(), port, [](hio_t* io)
		{
			auto thiz = static_cast<TcpServer*>(hevent_userdata(io));
			thiz->onAccept(io);
		});

	hevent_set_userdata(io, this);
	this->mInst = io;

	spdlog::info("start tcp server:{}:{}", host, port);
}

TcpServer::~TcpServer()
{
	this->close();
}

int32_t TcpServer::getServiceId()
{
	return this->mServiceId;
}

void TcpServer::setServiceId(int32_t value)
{
	this->mServiceId = value;
}

void TcpServer::setOnAccept(std::function<void(TcpServerSession*)> func)
{
	this->mOnAcceptFunc = func;
}

void TcpServer::onAccept(hio_t* io)
{
	auto fd = hio_fd(io);
	auto session = std::make_shared<TcpServerSession>(this,io, this->mId);
	this->mSessions.insert({ fd,session });

	if (this->mOnAcceptFunc)
	{
		this->mOnAcceptFunc(session.get());
	}
}

void TcpServer::send(int32_t fd, std::shared_ptr<std::vector<uint8_t>> data)
{
	auto iter = this->mSessions.find(fd);
	if (iter == this->mSessions.end())
	{
		return;
	}

	iter->second->write(data);
}

void TcpServer::closeFd(int32_t fd)
{
	auto iter = this->mSessions.find(fd);
	if (iter == this->mSessions.end())
	{
		return;
	}

	this->mSessions.erase(iter);
}

void TcpServer::close()
{
	this->mSessions.clear();

	if (this->mInst)
	{
		char addr[64] = { 0 };
		SOCKADDR_STR(hio_localaddr(this->mInst), addr);
		spdlog::info("stop tcp server:{}", addr);

		hio_close(this->mInst);
		this->mInst = nullptr;
	}
}