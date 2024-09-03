#include "serviceMgr.hpp"
#include<service/service.hpp>

extern ThreadQueue<std::shared_ptr<Msg>> GlobalMsgQueue;

ServiceMgr* ServiceMgr::mInst = nullptr;

ServiceMgr* ServiceMgr::getInstance()
{
	if (!ServiceMgr::mInst)
	{
		ServiceMgr::mInst = new ServiceMgr();
	}

	return ServiceMgr::mInst;
}

ServiceMgr::ServiceMgr()
{

}

ServiceMgr::~ServiceMgr()
{

}

uint32_t ServiceMgr::newService(std::string name, std::string filePath, bool isUnique)
{
	this->mServiceLock.lock();

	if (isUnique)
	{
		auto iter = this->mUniqueService.find(name);
		if (iter != this->mUniqueService.end())
		{
			auto serviceId = iter->second;
			this->mServiceLock.unlock();
			return serviceId;
		}
	}

	auto id = this->mAutoId;
	this->mAutoId++;
	auto service = std::make_shared<Service>(id, name,filePath);
	this->mServices.insert({ id,service });
	this->mServiceIter = this->mServices.begin();

	if (isUnique)
	{
		this->mUniqueService.insert({ name,id });
	}

	this->mServiceLock.unlock();

	return id;
}

uint32_t ServiceMgr::getService(std::string name)
{
	this->mServiceLock.lock();
	for (auto it = this->mServices.begin(); it != this->mServices.end(); ++it)
	{
		if (it->second->getName() == name)
		{
			auto id = it->second->getId();

			this->mServiceLock.unlock();
			return id;
		}
	}

	this->mServiceLock.unlock();
	return 0;
}

void ServiceMgr::destroyService(uint32_t id)
{
	this->mServiceLock.lock();

	auto iter = this->mServices.find(id);
	if (iter == this->mServices.end())
	{
		this->mServiceLock.unlock();
		return;
	}

	auto serviceName = iter->second->getName();

	this->mServices.erase(iter);
	this->mServiceIter = this->mServices.begin();

	auto uniqueIter = this->mUniqueService.find(serviceName);
	if (uniqueIter != this->mUniqueService.end())
	{
		this->mUniqueService.erase(uniqueIter);
	}

	this->mServiceLock.unlock();
}

void ServiceMgr::poll()
{
	this->mServiceLock.lock();
	if (this->mServices.size() == 0)
	{
		this->mServiceLock.unlock();
		return;
	}

	std::shared_ptr<Service> service = nullptr;
	service = this->mServiceIter->second;
	this->mServiceIter++;
	if (this->mServiceIter == this->mServices.end())
	{
		this->mServiceIter = this->mServices.begin();
	}

	this->mServiceLock.unlock();
	service->poll();
}

void ServiceMgr::send(uint32_t nodeId, std::shared_ptr<Msg> msg)
{
	GlobalMsgQueue.lock();
	GlobalMsgQueue.push(msg);
	GlobalMsgQueue.unlock();
}

void ServiceMgr::pushMsgs(uint32_t serviceId, std::list<std::shared_ptr<Msg>> &msgs)
{
	this->mServiceLock.lock();
	auto iter = this->mServices.find(serviceId);
	if (iter == this->mServices.end())
	{
		return;
	}

	auto& service = iter->second;
	this->mServiceLock.unlock();

	service->pushMsgs(msgs);
}
