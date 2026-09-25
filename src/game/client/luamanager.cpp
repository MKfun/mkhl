#include "luamanager.h"
#include "FileSystem.h"

#include <lua.hpp>

#include <tier2/tier2.h>

lua_State *gLuaState = nullptr;

extern cl_enginefunc_t gEngfuncs;

void CLuaManager::initLua(lua_State *&state)
{

	if (state)
		return;
	// we call it once, but who knows,
	// what goldsrc does under the hood? :)

	state = luaL_newstate();

	if (!state)
	{
		ConPrintf(ConColor::Red, "LuaJIT error.\n");
		return;
	}
	// if (!state)
	luaL_openlibs(state);

	// char gamedir[256];

	// g_pFullFileSystem->GetCurrentDirectory(gamedir, 256);
}

void CLuaManager::shutdownLua(lua_State *&state)
{
	if (state)
		lua_close(state);
	// else lua isn loaded
}

void CLuaManager::loadFile(lua_State *&state, const char *relativePath, const char *pathID = "GAME")
{

	// I CANT ESCAPE STD::STRING :cry::cry::cry: cuz im too lazy
	// todo: get rid of them

	FileHandle_t file = g_pFullFileSystem->Open(relativePath, "rb", pathID);
	if (!file)
	{
		return;
	}

	int size = g_pFullFileSystem->Size(file);
	char *buffer = (char *)malloc(size);
	g_pFullFileSystem->Read(buffer, size, file);
	g_pFullFileSystem->Close(file);
	std::string chunkName = "@" + std::string(relativePath);

	int loadStatus = luaL_loadbuffer(state, buffer, size, chunkName.c_str());
	free(buffer);
	if (loadStatus != 0)
	{
		const char *err = lua_tostring(state, -1);
		ConPrintf("Lua error %s\n", err);
		lua_pop(state, 1);
		return;
	}
	if (lua_pcall(state, 0, LUA_MULTRET, 0) != 0)
	{
		const char *err = lua_tostring(state, -1);
		ConPrintf("Lua runtime error: %s\n", err);
		lua_pop(state, 1);
		return;
	}
}

CON_COMMAND(lua_dostring, "Evaluate lua line")
{
	if (!gLuaState)
	{
		ConPrintf(ConColor::Yellow, "Lua wasnt initialized (properly or not)!\n");
		return;
	}

	int argc = gEngfuncs.Cmd_Argc();
	if (argc < 2)
	{
		ConPrintf("Usage: lua_dostring <lua code>");
		return;
	}

	std::string luaCode;
	for (int i = 1; i < argc; ++i)
	{
		const char *arg = gEngfuncs.Cmd_Argv(i);
		if (arg)
		{
			if (!luaCode.empty())
				luaCode += " ";
			luaCode += arg;
		}
	}

	if (luaL_dostring(gLuaState, luaCode.c_str()) != 0)
	{
		// При ошибке описание кладется на вершину стека Lua
		const char *err = lua_tostring(gLuaState, -1);
		ConPrintf(ConColor::Red, "Lua error: %s\n", err ? err : "unknown");
		lua_pop(gLuaState, 1);
	}
}

CON_COMMAND(lua_dofile, "Run .lua file")
{
	if (!gLuaState)
	{
		ConPrintf(ConColor::Yellow, "Lua wasnt initialized (properly or not)!\n");
		return;
	}

	int argc = gEngfuncs.Cmd_Argc();
	if (argc < 2)
	{
		ConPrintf("Usage: lua_dofile <lua file in gamedir>\n");
		return;
	}

	CLuaManager::loadFile(gLuaState, gEngfuncs.Cmd_Argv(1));
}

DLL_EXPORT cl_enginefunc_t *GetEngineAPI()
{
	return &gEngfuncs;
}
