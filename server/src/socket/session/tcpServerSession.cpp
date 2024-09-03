#include"tcpServerSession.hpp"
#include<hv/hsocket.h>
#include<spdlog/spdlog.h>
#include<socket/tcp/tcpServer.hpp>

TcpServerSession::TcpServerSession(TcpServer* server,hio_t* io,uint64_t serverId)
	:mParent(server),mInst(io), mServerId(serverId)
{
	char addr[64] = { 0 };
	SOCKADDR_STR(hio_peeraddr(io), addr);

	spdlog::info("client connect:{}", addr);

	hio_setcb_close(io, [](hio_t* io)
		{
			auto thiz = static_cast<TcpServerSession*>(hevent_userdata(io));
			if (!thiz)
			{
				return;
			}

			thiz->onClose();
		});
	hio_setcb_read(io, [](hio_t* io, void* buf, int readbytes)
		{
			auto thiz = static_cast<TcpServerSession*>(hevent_userdata(io));
			if (!thiz)
			{
				return;
			}

			auto data = std::make_shared<std::vector<uint8_t>>(readbytes + 1);
			memcpy(data->data(), buf, readbytes);
			thiz->onRead(data);
		});
	hevent_set_userdata(io, this);
	hio_read_start(io);
}

TcpServerSession::~TcpServerSession()
{
	hio_setcb_close(this->mInst, [](hio_t* io)
		{
			char addr[64] = { 0 };
			SOCKADDR_STR(hio_peeraddr(io), addr);
			spdlog::info("server close client:{}", addr);
		});

	hio_setcb_read(this->mInst, [](hio_t* io, void* buf, int readbytes) {});
	if (!this->isClose())
	{
		hio_close(this->mInst);
		this->mInst = nullptr;
	}
}

int32_t TcpServerSession::getFd()
{
	return hio_fd(this->mInst);
}

bool TcpServerSession::isClose()
{
	return hio_is_closed(this->mInst);
}

void TcpServerSession::setOnRead(std::function<void(TcpServerSession*, std::shared_ptr<std::vector<uint8_t>>)> func)
{
	this->mOnReadFunc = func;
}

void TcpServerSession::onRead(std::shared_ptr<std::vector<uint8_t>> data)
{
	if (this->mOnReadFunc)
	{
		this->mOnReadFunc(this,data);
	}
}

void TcpServerSession::write(std::shared_ptr<std::vector<uint8_t>> data)
{
	hio_write(this->mInst, data->data(), data->size());
}

void TcpServerSession::write(std::string data)
{
	hio_write(this->mInst, data.data(), data.length());
}

void TcpServerSession::onClose()
{
	char addr[64] = { 0 };
	SOCKADDR_STR(hio_peeraddr(this->mInst), addr);
	spdlog::info("client close:{}", addr);

	auto fd = hio_fd(this->mInst);
	this->mParent->closeFd(fd);
}
