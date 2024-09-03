#pragma once
#include<hv/hloop.h>
#include<memory>
#include<vector>
#include<string>
#include<functional>

class TcpServer;

class TcpServerSession
{
public:
	TcpServerSession(TcpServer* server, hio_t* io, uint64_t serverId);
	~TcpServerSession();
public:
	int32_t getFd();

public:
	bool isClose();

public:
	void setOnRead(std::function<void(TcpServerSession*, std::shared_ptr<std::vector<uint8_t>>)> func);
	void onRead(std::shared_ptr<std::vector<uint8_t>> data);

public:
	void write(std::shared_ptr<std::vector<uint8_t>> data);
	void write(std::string data);

public:
	void close();
	void onClose();

private:
	TcpServer* mParent = nullptr;
	hio_t* mInst = nullptr;
	uint64_t mServerId = 0;
	std::function<void(TcpServerSession*,std::shared_ptr<std::vector<uint8_t>>)> mOnReadFunc;
};