#pragma once
#include<map>
#include <string>
#include <memory>
#include <mutex>
#include <list>
#include<msg/msg.hpp>

class Service;

class ServiceMgr
{
public:
	static ServiceMgr* mInst;
	static ServiceMgr* getInstance();

private:
	ServiceMgr();
	virtual ~ServiceMgr();

public:
	uint32_t newService(std::string name, std::string filePath, bool isUnique = false);
	uint32_t getService(std::string name);
	void destroyService(uint32_t id);

public:
	void poll();

public:
	void pushMsgs(uint32_t serviceId, std::list<std::shared_ptr<Msg>>& msgs);

public:
	void send(uint32_t nodeId, std::shared_ptr<Msg> msg);

private:
	std::map<uint32_t, std::shared_ptr<Service>>::iterator mServiceIter;
	std::map<uint32_t,std::shared_ptr<Service>> mServices;
	std::map<std::string, uint32_t> mUniqueService;
	uint32_t mAutoId = 1;
	std::mutex mServiceLock;
};