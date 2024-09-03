#pragma once
#include<lua/luaApi.hpp>
#include<memory>
#include<msg/msg.hpp>
#include<queue/threadQueue.hpp>
#include<list>
#include<map>

class Service
{
public:
	Service(uint64_t id, std::string &name, std::string & basePath);
	virtual ~Service();

public:
	bool isInit()const;

public:
	void poll();

public:
	void pushMsgs(std::list<std::shared_ptr<Msg>>& msgs);

public:
	uint64_t getId()const;
	const std::string& getName()const;

private:
	uint64_t mId;
	std::string mName;
	std::string mFilePath;
	ThreadQueue<std::shared_ptr<Msg>> mMsgs;
	bool mIsInit = false;
	float mMsgSplit = 1.0f;

private:
	lua_State* mState = nullptr;
};