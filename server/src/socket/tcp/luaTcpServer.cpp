#include"tcpServerMgr.hpp"
#include<lua.hpp>

int newTcpServer(lua_State * state)
{
	auto host = lua_tostring(state, 1);
	auto port = lua_tointeger(state, 2);

	lua_settop(state, 0);
	auto serviceId = 0;
	lua_getglobal(state, "SERVICE_ID");

	if (lua_isinteger(state, -1))
	{
		serviceId = lua_tointeger(state, -1);
	}

	auto tcpServerMgr = TcpServerMgr::getInstance();
	auto id=tcpServerMgr->newServer(host, port, serviceId);
	lua_pushinteger(state, id);

	return 1;
}

int destroyTcpServer(lua_State* state)
{
	auto id = lua_tointeger(state, 1);
	auto tcpServerMgr = TcpServerMgr::getInstance();
	tcpServerMgr->destroyServer(id);

	return 0;
}

int sendTcpServer(lua_State* state)
{
	auto id = lua_tointeger(state, 1);
	auto fd = lua_tointeger(state, 2);
	auto dataLen = lua_tointeger(state, 3);
	if (dataLen <= 0)
	{
		lua_settop(state, 0);
		return 0;
	}

	auto data = std::make_shared<std::vector<uint8_t>>(dataLen);
	auto luaData = lua_tostring(state, 4);
	memcpy(data->data(), luaData, dataLen);

	auto tcpServerMgr = TcpServerMgr::getInstance();
	tcpServerMgr->send(id, fd, data);

	return 0;
}

int tcpServerCloseFd(lua_State* state)
{
	auto id = lua_tointeger(state, 1);
	auto fd = lua_tointeger(state, 2);

	auto tcpServerMgr = TcpServerMgr::getInstance();
	tcpServerMgr->closeFd(id, fd);

	return 0;
}

void luaRegisterTcpServerMgrAPI(lua_State* state)
{
	lua_settop(state, 0);

	auto hasTable = true;
	lua_getglobal(state, "API");
	if (lua_isnil(state, -1))
	{
		lua_newtable(state);
		hasTable = false;
	}

	lua_pushcfunction(state,newTcpServer);
	lua_setfield(state, -2, "TcpServer_New");

	lua_pushcfunction(state, destroyTcpServer);
	lua_setfield(state, -2, "TcpServer_Destroy");

	lua_pushcfunction(state, sendTcpServer);
	lua_setfield(state, -2, "TcpServer_Send");

	lua_pushcfunction(state, tcpServerCloseFd);
	lua_setfield(state, -2, "TcpServer_CloseFd");

	if (!hasTable)
	{
		lua_setglobal(state, "API");
	}
	else
	{
		lua_pop(state, 1);
	}
}
