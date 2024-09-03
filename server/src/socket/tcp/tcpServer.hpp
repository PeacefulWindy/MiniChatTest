#pragma once
#include<hv/hloop.h>
#include <string>
#include <memory>
#include <map>
#include<functional>

class TcpServerSession;

class TcpServer
{
public:
	TcpServer(int32_t id,std::string &host, int port);
	virtual ~TcpServer();

public:
	int32_t getServiceId();
	void setServiceId(int32_t value);

public:
	void setOnAccept(std::function<void(TcpServerSession*)> func);
	void onAccept(hio_t* io);

public:
	void send(int32_t fd, std::shared_ptr<std::vector<uint8_t>> data);

public:
	void closeFd(int32_t fd);
	void close();

private:
	int32_t mId = 0;
	std::map<int32_t, std::shared_ptr<TcpServerSession>> mSessions;
	hio_t* mInst = nullptr;
	std::function<void(TcpServerSession*)> mOnAcceptFunc;
	int32_t mServiceId = 0;
};