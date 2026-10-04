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
	luaL_openlibs(state);

	char gamedir[256] = {0};
	const char *gameDirName = (gEngfuncs.pfnGetGameDirectory) ? gEngfuncs.pfnGetGameDirectory() : nullptr;
	if (gameDirName && gameDirName[0] != '\0')
	{
		strncpy(gamedir, gameDirName, sizeof(gamedir) - 1);
		gamedir[sizeof(gamedir) - 1] = '\0';
	}
	else if (g_pFullFileSystem)
	{
		g_pFullFileSystem->GetCurrentDirectory(gamedir, sizeof(gamedir));
	}
	else
	{
		strncpy(gamedir, ".", sizeof(gamedir) - 1);
		gamedir[sizeof(gamedir) - 1] = '\0';
	}

	// package.path = <gamedir>/luascripts/?.lua;<gamedir>/luascripts/?/init.lua
	char packagePath[2048];
	lua_getglobal(state, "package");
	lua_getfield(state, -1, "path");
	const char *oldPathRaw = lua_tostring(state, -1);

	if (oldPathRaw && oldPathRaw[0] != '\0')
	{
		snprintf(packagePath, sizeof(packagePath),
			"%s/luascripts/?.lua;%s/luascripts/?/init.lua;%s",
			gamedir, gamedir, oldPathRaw);
	}
	else
	{
		snprintf(packagePath, sizeof(packagePath),
			"%s/luascripts/?.lua;%s/luascripts/?/init.lua",
			gamedir, gamedir);
	}
	lua_pop(state, 1);

	lua_pushstring(state, packagePath);
	lua_setfield(state, -2, "path");
	lua_pop(state, 1);
}

void CLuaManager::shutdownLua(lua_State *&state)
{
	if (state)
		lua_close(state);
	// else lua isn't loaded
}

void CLuaManager::loadFile(lua_State *&state, const char *relativePath, const char *pathID = "GAME")
{
	if (!g_pFullFileSystem)
	{
		ConPrintf("Lua loadFile error: filesystem not initialized\n");
		return;
	}

	FileHandle_t file = g_pFullFileSystem->Open(relativePath, "rb", pathID);
	if (!file)
		return;

	int size = g_pFullFileSystem->Size(file);
	char *buffer = (char *)malloc(size);
	g_pFullFileSystem->Read(buffer, size, file);
	g_pFullFileSystem->Close(file);

	char chunkName[512];
	snprintf(chunkName, sizeof(chunkName), "@%s", relativePath);

	int loadStatus = luaL_loadbuffer(state, buffer, size, chunkName);
	free(buffer);
	if (loadStatus != 0)
	{
		const char *err = lua_tostring(state, -1);
		ConPrintf("Lua error %s\n", err);
		lua_pop(state, 1);
		return;
	}

	char absScriptFile[512];
	absScriptFile[0] = '\0';
	g_pFullFileSystem->GetLocalPath(relativePath, absScriptFile, sizeof(absScriptFile));

	char absScriptDir[512];
	absScriptDir[0] = '\0';
	if (absScriptFile[0] != '\0')
	{
		snprintf(absScriptDir, sizeof(absScriptDir), "%s", absScriptFile);
		char *lastSlash = strrchr(absScriptDir, '/');
		char *lastBackslash = strrchr(absScriptDir, '\\');
		char *sep = lastSlash > lastBackslash ? lastSlash : lastBackslash;
		if (sep)
			*sep = '\0';
		else
			absScriptDir[0] = '\0';
	}

	lua_getglobal(state, "package");
	lua_getfield(state, -1, "path");
	const char *oldPathRaw = lua_tostring(state, -1);

	char oldPath[2048];
	snprintf(oldPath, sizeof(oldPath), "%s", oldPathRaw ? oldPathRaw : "");
	lua_pop(state, 1);

	if (absScriptDir[0] != '\0')
	{
		char newPath[4096];
		snprintf(newPath, sizeof(newPath),
			"%s/?.lua;%s/?/init.lua;%s",
			absScriptDir, absScriptDir, oldPath);

		lua_pushstring(state, newPath);
		lua_setfield(state, -2, "path");
	}
	lua_pop(state, 1); // pop package

	if (lua_pcall(state, 0, LUA_MULTRET, 0) != 0)
	{
		const char *err = lua_tostring(state, -1);
		ConPrintf("Lua runtime error: %s\n", err);
		lua_pop(state, 1);
	}

	lua_getglobal(state, "package");
	lua_pushstring(state, oldPath);
	lua_setfield(state, -2, "path");
	lua_pop(state, 1);
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

	char luaCode[4096];
	luaCode[0] = '\0';
	for (int i = 1; i < argc; ++i)
	{
		const char *arg = gEngfuncs.Cmd_Argv(i);
		if (!arg)
			continue;
		if (i > 1)
			strncat(luaCode, " ", sizeof(luaCode) - strlen(luaCode) - 1);
		strncat(luaCode, arg, sizeof(luaCode) - strlen(luaCode) - 1);
	}

	if (luaL_dostring(gLuaState, luaCode) != 0)
	{
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
