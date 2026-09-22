#include "rkhud_scoreboard.h"

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#pragma pop_macro("Assert")

#include <algorithm>
#include <tier1/strtools.h>
#include "hud.h"
#include "cl_util.h"
#include "player_info.h"
#include "vgui/client_viewport.h"

RkHudScoreboard RkHudScoreboard::m_Instance = RkHudScoreboard("hud_scoreboard");

// Struct layout for data-binding model.
struct PlayerEntry
{
	int entid;
	Rml::String name;
	Rml::String steamid;
	Rml::String avatar;
	int frags;
	int deaths;
	int ping;
	bool thisplayer;
};

struct ScoreboardData
{
	Rml::String serverName;
	Rml::String mapName;
	int numPlayers;
	int numSpecs;
	Rml::Vector<PlayerEntry> players;
} scoreboardData;

documentReloadFuncs scoreboardDocReloadFuncs;

void UnloadRkScoreboard()
{
	RkHudScoreboard &pScoreboard = RkHudScoreboard::m_Instance;
	if (!pScoreboard.m_pInstance)
		return;

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (hudCtx)
	{
		hudCtx->RemoveDataModel("scoreboard_model");
		pScoreboard.m_dataModel = nullptr;
	}
	else
	{
		Warning("Couldn't access hudCtx to unload scoreboard datamodel\n");
	}

	pScoreboard.m_pInstance->Close();
	pScoreboard.m_pInstance = nullptr;
	pScoreboard.m_bVisible = false;
}

void LoadRkScoreboard()
{
	scoreboardDocReloadFuncs.LoadDocument = &LoadRkScoreboard;
	scoreboardDocReloadFuncs.UnloadDocument = &UnloadRkScoreboard;

	RkHudScoreboard &pScoreboard = RkHudScoreboard::m_Instance;

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (!hudCtx)
	{
		Error("Couldn't access hudctx!\n");
		return;
	}

	if (pScoreboard.m_pInstance || pScoreboard.m_dataModel)
	{
		Warning("RkHudScoreboard already loaded, call unload first!\n");
		return;
	}

	Rml::DataModelConstructor constructor = hudCtx->CreateDataModel("scoreboard_model");
	if (!constructor)
	{
		Error("Couldn't create datamodel for scoreboard!\n");
		return;
	}

	static bool is_struct_registered = false;
	if (!is_struct_registered)
	{
		if (auto playerentry_handle = constructor.RegisterStruct<PlayerEntry>())
		{
			playerentry_handle.RegisterMember("entid", &PlayerEntry::entid);
			playerentry_handle.RegisterMember("name", &PlayerEntry::name);
			playerentry_handle.RegisterMember("steamid", &PlayerEntry::steamid);
			playerentry_handle.RegisterMember("avatar", &PlayerEntry::avatar);
			playerentry_handle.RegisterMember("frags", &PlayerEntry::frags);
			playerentry_handle.RegisterMember("kills", &PlayerEntry::frags);
			playerentry_handle.RegisterMember("deaths", &PlayerEntry::deaths);
			playerentry_handle.RegisterMember("ping", &PlayerEntry::ping);
			playerentry_handle.RegisterMember("thisplayer", &PlayerEntry::thisplayer);
			is_struct_registered = true;
		}
	}

	static bool arrays_registered = false;
	if (!arrays_registered)
	{
		constructor.RegisterArray<Rml::Vector<PlayerEntry>>();
		arrays_registered = true;
	}

	constructor.Bind("players", &scoreboardData.players);
	constructor.Bind("server_name", &scoreboardData.serverName);
	constructor.Bind("map_name", &scoreboardData.mapName);
	constructor.Bind("num_players", &scoreboardData.numPlayers);
	constructor.Bind("num_specs", &scoreboardData.numSpecs);

	pScoreboard.m_dataModel = constructor.GetModelHandle();

	pScoreboard.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud("body", "GAME", "rocketui/hud_scoreboard.rml", &scoreboardDocReloadFuncs);

	if (!pScoreboard.m_pInstance)
	{
		Error("Couldn't create hud_scoreboard document!\n");
		return;
	}

	pScoreboard.m_pInstance->Hide();
	pScoreboard.m_bVisible = false;
}

RkHudScoreboard::RkHudScoreboard(const char *value)
	: m_bVisible(false)
	, m_pInstance(nullptr)
{
}

RkHudScoreboard::~RkHudScoreboard() noexcept
{
	UnloadRkScoreboard();
}

void RkHudScoreboard::LevelInit()
{
	LoadRkScoreboard();
}

void RkHudScoreboard::LevelShutdown()
{
	scoreboardData.players.clear();
	UnloadRkScoreboard();
}

void RkHudScoreboard::ShowPanel(bool bShow, bool force)
{
	if (!m_pInstance)
		return;

	if (bShow)
	{
		if (!m_bVisible)
		{
			m_pInstance->Show();
			m_pInstance->PullToFront();

			if (Rml::Element *sb = m_pInstance->GetElementById("scoreboard"))
			{
				sb->RemoveProperty("animation");
				sb->SetProperty("animation", "scoreboard-appear 0.18s cubic-out");
			}
		}
		m_bVisible = true;
		Update();
	}
	else
	{
		if (m_bVisible)
		{
			m_pInstance->Hide();
			if (Rml::Element *sb = m_pInstance->GetElementById("scoreboard"))
			{
				sb->RemoveProperty("animation");
			}
		}
		m_bVisible = false;
	}
}

void RkHudScoreboard::SetActive(bool bActive)
{
	ShowPanel(bActive, false);
}

bool RkHudScoreboard::ShouldDraw()
{
	return 1;
}

void RkHudScoreboard::Update()
{
	if (!m_bVisible || !m_pInstance || !m_dataModel)
		return;

	scoreboardData.players.clear();
	scoreboardData.numSpecs = 0;
	scoreboardData.numPlayers = 0;

	if (g_pViewport && g_pViewport->GetServerName()[0])
	{
		scoreboardData.serverName = g_pViewport->GetServerName();
	}
	else
	{
		scoreboardData.serverName = "Half-Life";
	}

	char mapBuf[64] = {0};
	const char *levelName = gEngfuncs.pfnGetLevelName();
	if (levelName && levelName[0])
	{
		V_FileBase(levelName, mapBuf, sizeof(mapBuf));
		scoreboardData.mapName = mapBuf;
	}
	else
	{
		scoreboardData.mapName.clear();
	}

	for (int i = 1; i <= MAX_PLAYERS; i++)
	{
		CPlayerInfo *pi = GetPlayerInfo(i)->Update();
		if (!pi->IsConnected())
			continue;

		if (pi->IsSpectator())
		{
			scoreboardData.numSpecs++;
			continue;
		}

		PlayerEntry entry;
		entry.entid = i;
		entry.name = pi->GetDisplayName();
		entry.frags = pi->GetFrags();
		entry.deaths = pi->GetDeaths();
		entry.ping = pi->GetPing();
		entry.thisplayer = pi->IsThisPlayer();

		uint64 steamID64 = pi->GetValidSteamID64();
		const char *szSteamID = pi->GetSteamID();
		entry.steamid = (szSteamID && szSteamID[0]) ? szSteamID : "";

		char avatarBuf[64];
		snprintf(avatarBuf, sizeof(avatarBuf), "steamavatar://%llu", (unsigned long long)steamID64);
		entry.avatar = avatarBuf;

		scoreboardData.players.push_back(entry);
		scoreboardData.numPlayers++;
	}

	auto sortFunc = [](const PlayerEntry &a, const PlayerEntry &b) {
		if (a.frags != b.frags)
			return a.frags > b.frags;
		if (a.deaths != b.deaths)
			return a.deaths < b.deaths;
		return a.entid < b.entid;
	};

	std::sort(scoreboardData.players.begin(), scoreboardData.players.end(), sortFunc);

	m_dataModel.DirtyVariable("players");
	m_dataModel.DirtyVariable("server_name");
	m_dataModel.DirtyVariable("map_name");
	m_dataModel.DirtyVariable("num_players");
	m_dataModel.DirtyVariable("num_specs");
}