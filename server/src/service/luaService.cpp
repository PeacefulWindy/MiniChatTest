#include"service.hpp"
#include"serviceMgr.hpp"
#include<lua.hpp>

int newService(lua_State* state)
{
	auto serviceName = lua_tostring(state, 1);
	auto servicePath = lua_tostring(state, 2);
	auto isUnique = false;
	auto serviceMgr = ServiceMgr::getInstance();
	auto id = serviceMgr->newService(serviceName, servicePath, isUnique);
	lua_pushnumber(state, id);

	return 1;
}

int getService(lua_State* state)
{
	auto name = lua_tostring(state, 1);
	auto serviceMgr = ServiceMgr::getInstance();
	lua_pushinteger(state, serviceMgr->getService(name));
	return 1;
}

int removeService(lua_State* state)
{
	auto id = lua_tonumber(state, 1);
	auto serviceMgr = ServiceMgr::getInstance();
	serviceMgr->destroyService(id);

	return 0;
}

int send(lua_State* state)
{
	auto msg = std::make_shared<Msg>();

	auto protocol = lua_tointeger(state, 1);
	auto nodeId = lua_tointeger(state, 2);
	auto serviceId = lua_tointeger(state, 3);
	auto sessionId = lua_tointeger(state, 4);
	auto cmd = lua_tostring(state, 5);
	auto dataLen = static_cast<size_t>(lua_tonumber(state, 6));

	if (dataLen > 0)
	{
		auto data = std::make_shared<std::vector<uint8_t>>();
		data->resize(dataLen + 1);
		auto luaData = lua_tostring(state, 7);
		memcpy(data->data(), luaData, dataLen);
		msg->setData(data);
	}

	lua_settop(state, 0);
	lua_getglobal(state, "SERVICE_ID");
	auto fromServiceId = lua_tointeger(state, -1);

	msg->setProtocol(protocol);
	msg->setFromServiceId(fromServiceId);
	msg->setToServiceId(serviceId);
	msg->setSessionId(sessionId);
	msg->setCmd(cmd);

	ServiceMgr::getInstance()->send(nodeId, msg);
	return 0;
}

void luaRegisterServiceMgrAPI(lua_State* state)
{
	lua_settop(state, 0);

	auto hasTable = true;
	lua_getglobal(state, "API");
	if (lua_isnil(state, -1))
	{
		lua_newtable(state);
		hasTable = false;
	}

	lua_pushcfunction(state, newService);
	lua_setfield(state, -2, "Service_New");

	lua_pushcfunction(state, getService);
	lua_setfield(state, -2, "Service_GetService");

	lua_pushcfunction(state, removeService);
	lua_setfield(state, -2, "Service_RemoveService");

	lua_pushcfunction(state, send);
	lua_setfield(state, -2, "Service_Send");

	if (!hasTable)
	{
		lua_setglobal(state, "API");
	}
	else
	{
		lua_pop(state, 1);
	}
}