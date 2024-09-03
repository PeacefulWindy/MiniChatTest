#include "luaApi.hpp"
#include<spdlog/spdlog.h>

extern void luaRegisterJsonAPI(lua_State* state);
extern void luaRegisterServiceMgrAPI(lua_State* state);
extern void luaRegisterTcpServerMgrAPI(lua_State* state);
extern void luaRegisterMariadbAPI(lua_State* state);

int debug(lua_State* state)
{
	auto data = lua_tostring(state, 1);
	spdlog::debug("{}", data);
	return 0;
}

int info(lua_State* state)
{
	auto data = lua_tostring(state, 1);
	spdlog::info("{}", data);
	return 0;
}

int warning(lua_State* state)
{
	auto data = lua_tostring(state, 1);
	spdlog::warn("{}", data);
	return 0;
}

int error(lua_State * state)
{
	auto data = lua_tostring(state, 1);
	spdlog::error("{}", data);
	return 0;
}

void luaRegisterLogAPI(lua_State* state)
{
	lua_settop(state, 0);

	auto hasTable = true;
	lua_getglobal(state, "API");
	if (lua_isnil(state, -1))
	{
		lua_newtable(state);
		hasTable = false;
	}

	lua_pushcfunction(state, debug);
	lua_setfield(state, -2, "LogDebug");

	lua_pushcfunction(state, info);
	lua_setfield(state, -2, "LogInfo");

	lua_pushcfunction(state, warning);
	lua_setfield(state, -2, "LogWarning");

	lua_pushcfunction(state, error);
	lua_setfield(state, -2, "LogError");

	if (!hasTable)
	{
		lua_setglobal(state, "API");
	}
	else
	{
		lua_pop(state, 1);
	}
}

void luaPrintStack(lua_State* state)
{
	printf("==============Lua Stack ==============\n");
	auto size = lua_gettop(state);
	for (auto i = size; i > 0; i--)
	{
		auto typeId = lua_type(state, i);
		auto typeName = lua_typename(state, typeId);
		auto valueStr = std::string();
		switch (typeId)
		{
		case LUA_TNONE:
			valueStr = "none";
			break;
		case LUA_TNIL:
			valueStr = "nil";
			break;
		case LUA_TBOOLEAN:
			valueStr = std::to_string(lua_toboolean(state, i));
			break;
		case LUA_TLIGHTUSERDATA:
			valueStr = std::to_string((int64_t)lua_touserdata(state, i));
			break;
		case LUA_TNUMBER:
			valueStr = std::to_string(lua_tonumber(state, i));
			break;
		case LUA_TSTRING:
			valueStr = std::string(lua_tostring(state, i));
			break;
		case LUA_TTABLE:
			valueStr = "table";
			break;
		case LUA_TFUNCTION:
			valueStr = "function";
			break;
		case LUA_TUSERDATA:
			valueStr = std::to_string((int64_t)lua_touserdata(state, i));
			break;
		case LUA_TTHREAD:
			valueStr = "coroutine";
			break;
		}

		printf("%d %d (%s)%s\n", i - size - 1, i, typeName, valueStr.c_str());
	}
	printf("==============Lua Stack End==============\n\n");
}

void luaRegisterAPI(lua_State* state)
{
#ifdef _DEBUG
	lua_pushboolean(state, true);
#else
	lua_pushboolean(state, false);
#endif
	lua_setglobal(state, "DEBUG_MODE");

	luaRegisterLogAPI(state);
	luaRegisterJsonAPI(state);
	luaRegisterServiceMgrAPI(state);
	luaRegisterTcpServerMgrAPI(state);
	luaRegisterMariadbAPI(state);
}
