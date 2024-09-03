#pragma once
#include<lua.hpp>

void luaPrintStack(lua_State* state);

void luaRegisterAPI(lua_State* state);