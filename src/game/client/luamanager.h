#ifndef LUAMANAGER_H
#define LUAMANAGER_H

#include <lua.hpp>
class CLuaManager
{
public:
	// CLuaManager();
	static void initLua(lua_State *&state);
	static void shutdownLua(lua_State *&state);
	static void loadFile(lua_State *&state, const char *relativePath, const char *pathID);
};

#endif // LUAMANAGER_H
