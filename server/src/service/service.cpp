#include"service.hpp"
#include<spdlog/spdlog.h>

constexpr const char* SERVICE_BASE_PATH = "Resources/services/";

Service::Service(uint64_t id, std::string& name, std::string& basePath)
	:mId(id), mName(name), mFilePath(SERVICE_BASE_PATH + basePath + "/main.lua")
{
	spdlog::debug("{} service init!", this->mName);

	this->mState = luaL_newstate();
	luaL_openlibs(this->mState);

	lua_getglobal(this->mState, "package");

	auto strServicePath = std::string(SERVICE_BASE_PATH);
	auto path = strServicePath + "?.lua;";
	path = strServicePath + basePath + "/?.lua;" + path;
	path = path + "Resources/lualib/?.lua;";
	path = path + strServicePath + "define/?.lua";

	lua_pushstring(this->mState, path.c_str());
	lua_setfield(this->mState, -2, "path");

	auto cPath = std::string("lib/?.so;lib/?.dll");
	lua_pushstring(this->mState, cPath.c_str());
	lua_setfield(this->mState, -2, "cpath");

	lua_pop(this->mState, 1);

	lua_pushinteger(this->mState, this->mId);
	lua_setglobal(this->mState, "SERVICE_ID");

	lua_pushlightuserdata(this->mState, this);
	lua_setglobal(this->mState, "SERVICE_PTR");

	luaRegisterAPI(this->mState);

	lua_getglobal(this->mState, "require");
	lua_pushstring(this->mState, "luaApi");
	auto type = lua_type(this->mState, -2);
	if (lua_pcall(this->mState, 1, 0, -2) != LUA_OK)
	{
		spdlog::error("{}", lua_tostring(this->mState, -1));
		return;
	}

	if (luaL_dofile(this->mState,this->mFilePath.c_str()) != LUA_OK)
	{
		spdlog::error("{}", lua_tostring(this->mState, -1));
		return;
	}

	lua_getglobal(this->mState, "start");
	if (!lua_isfunction(this->mState,-1))
	{
		spdlog::warn("{} not found start function.", this->mFilePath.c_str());
		return;
	}

	if (lua_pcall(this->mState, 0, 0, -1) != LUA_OK)
	{
		spdlog::error("{}", lua_tostring(this->mState, -1));
		return;
	}

	this->mIsInit = true;
	spdlog::info("{} service started!", this->mName);
	lua_settop(this->mState, -1);
}

Service::~Service()
{
	if (!this->isInit())
	{
		return;
	}

	lua_getglobal(this->mState, "stop");
	if (lua_type(this->mState, -1) != LUA_TFUNCTION)
	{
		spdlog::warn("{} not found stop function.", this->mFilePath.c_str());
		return;
	}

	if (lua_pcall(this->mState, 0, 0, 0) != LUA_OK)
	{
		spdlog::error("{}", lua_tostring(this->mState, -1));
		return;
	}

	this->mIsInit = false;
	spdlog::info("{} service stop!", this->mName);
}

bool Service::isInit() const
{
	return this->mIsInit;
}

void Service::poll()
{
	if (!this->isInit())
	{
		return;
	}
	this->mMsgs.lock();

	auto size = this->mMsgs.getSize();
	if (size == 0)
	{
		this->mMsgs.unlock();
		return;
	}

	lua_settop(this->mState, 0);
	auto minSize = static_cast<size_t>(size / this->mMsgSplit);
	for (auto i = 0; i < minSize; i++)
	{
		auto msg = this->mMsgs.getFront();

		lua_settop(this->mState, 0);
		lua_getglobal(this->mState, "LuaAPI");
		if (lua_istable(this->mState, -1))
		{
			lua_getfield(this->mState, -1, "onMsg");

			if (lua_isfunction(this->mState, 2))
			{
				lua_pushinteger(this->mState, msg->getProtocol());
				lua_pushinteger(this->mState, 0);
				lua_pushinteger(this->mState, msg->getFromServiceId());
				lua_pushinteger(this->mState, msg->getSessionId());
				auto cmd = msg->getCmd();
				lua_pushstring(this->mState, cmd.c_str());
				auto data = msg->getData();
				lua_pushstring(this->mState, reinterpret_cast<const char*>(data->data()));


				if (lua_pcall(this->mState, 6, 0, 2) != LUA_OK)
				{
					spdlog::error("{}", lua_tostring(this->mState, -1));
				}
			}
		}
		this->mMsgs.pop();
	}

	this->mMsgs.unlock();
}

void Service::pushMsgs(std::list<std::shared_ptr<Msg>>& msgs)
{
	if (!this->isInit())
	{
		return;
	}

	this->mMsgs.lock();

	for (auto it = msgs.begin(); it != msgs.end(); ++it)
	{
		this->mMsgs.push(*it);
	}

	this->mMsgs.unlock();
}

uint64_t Service::getId() const
{
	return this->mId;
}

const std::string& Service::getName() const
{
	return this->mName;
}