#pragma once
#include<mutex>
#include<map>
#include<vector>

class TcpServer;
class TcpServerSession;
class Msg;

class TcpServerMgr
{
public:
	static TcpServerMgr* mInst;
	static TcpServerMgr* getInstance();

public:
	int32_t newServer(std::string host, int32_t port, int32_t serviceId = 0);
	void destroyServer(int32_t id);

public:
	void send(int32_t id, int32_t fd, std::shared_ptr<std::vector<uint8_t>> data);

public:
	void closeFd(int32_t id, int32_t fd);

private:
	std::map<int32_t, std::shared_ptr<TcpServer>> mServers;
	std::mutex mLock;
	int32_t mAutoId = 1;
};