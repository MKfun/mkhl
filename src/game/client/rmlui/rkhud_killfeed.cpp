#include "rkhud_killfeed.h"

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#include "hud.h"
ConVar rocket_hud_killfeed_linger_time("rocket_hud_killfeed_linger_time", "7", 0, "How long in seconds to keep each killfeed entry on screen.");
struct KillfeedEntry
{
	Rml::String attackerName;
	Rml::String gunName;
	Rml::String victimName;
	bool headshot;
	bool wallbang;
	bool inair;
	float noticeSpawnTime;
	bool you_killed;
	bool killed_you;
};

// Struct layout for data-binding model.
struct KillFeedData
{
	std::deque<KillfeedEntry> entries;
} killFeedData;

CON_COMMAND(rocket_hud_killfeed_clear, "")
{
	if (killFeedData.entries.empty())
		return;
	// pop off the first guy, then we're done. This gets called often enough to not matter about the rest.
	// if ((gHUD.m_flTime - killFeedData.entries.front().noticeSpawnTime) > rocket_hud_killfeed_linger_time.GetFloat())
	// {
	for (int i = 0; i < killFeedData.entries.size(); i++)
		killFeedData.entries.pop_front();
	RkHudKillfeed::m_Instance.m_dataModel.DirtyVariable("killfeed_entries");
	// m_dataModel.Update();
	// }
}

enum KillRarity
{
	KILLRARITY_HEADSHOT = 0x001, // The killer player kills the victim with a headshot
	KILLRARITY_NOSCOPE = 0x002, // The killer player kills the victim with a sniper rifle with no scope
	KILLRARITY_PENETRATED = 0x004, // The killer player kills the victim through walls
	KILLRARITY_INAIR = 0x008
};

void RkHudKillfeed::OnPlayerDeath(int killerId, int victimId, const char *killedwith, int killrarity)
{
	KillfeedEntry entry;
	if (!GetThisPlayerInfo())
	{
		// Not yet connected
		return;
	}
	CPlayerInfo *killer = GetPlayerInfoSafe(killerId);
	CPlayerInfo *victim = GetPlayerInfoSafe(victimId);
	int thisPlayerId = GetThisPlayerInfo()->GetIndex();

	bool killedu = thisPlayerId == victimId;
	bool ukilled = thisPlayerId == killerId;
	entry.attackerName = killer ? killer->GetName() : "";
	entry.victimName = victim ? victim->GetName() : "";
	entry.headshot = (killrarity & KILLRARITY_HEADSHOT);
	// entry.gunName = killedwith;
	char icon_path[MAX_PATH];
	sprintf(icon_path, "images/killfeed/%s.svg", killedwith);
	entry.gunName = icon_path;
	entry.noticeSpawnTime = gHUD.m_flTime;
	entry.wallbang = (killrarity & KILLRARITY_PENETRATED);
	entry.killed_you = killedu;
	entry.you_killed = ukilled;
	entry.inair = (killrarity & KILLRARITY_INAIR);
	killFeedData.entries.push_back(entry);
	m_dataModel.DirtyVariable("killfeed_entries");
}
// called every frame
void RkHudKillfeed::CheckForOldEntries()
{
	if (killFeedData.entries.empty())
		return;

	bool bChanged = false;
	while (!killFeedData.entries.empty() && (gHUD.m_flTime - killFeedData.entries.front().noticeSpawnTime) > rocket_hud_killfeed_linger_time.GetFloat())
	{
		killFeedData.entries.pop_front();
		bChanged = true;
	}

	if (bChanged)
	{
		m_dataModel.DirtyVariable("killfeed_entries");
	}
}

void RkHudKillfeed::Update(void)
{
	if (m_bVisible)
	{
		CheckForOldEntries();
	}
}

RkHudKillfeed RkHudKillfeed::m_Instance = RkHudKillfeed("hud_killfeed");
void UnloadRkKillFeed()
{
	RkHudKillfeed &pKillFeed = RkHudKillfeed::m_Instance;
	if (!pKillFeed.m_pInstance)
		return; // Not loaded, nothing to unload


	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (hudCtx)
	{
		hudCtx->RemoveDataModel("killfeed_model");
		pKillFeed.m_dataModel = nullptr;
	}
	else
	{
		Warning("couldn't access hudctx to unload killfeed datamodel!\n");
	}

	pKillFeed.m_pInstance->Close();
	pKillFeed.m_pInstance = nullptr;
	pKillFeed.m_bVisible = false;
}
documentReloadFuncs kfDocReload;
void LoadRkKillFeed()
{
	kfDocReload.UnloadDocument = &UnloadRkKillFeed;
	kfDocReload.LoadDocument = &LoadRkKillFeed;
	RkHudKillfeed &pKillFeed = RkHudKillfeed::m_Instance;
	// if (!pKillFeed.m_pInstance)
	// {
	// 	Warning("Couldn't grab hud killfeed to load!\n");
	// 	return;
	// }

	Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
	if (!hudCtx)
	{
		Error("Couldn't access hudctx!\n");
		/* Exit */
	}

	if (pKillFeed.m_pInstance || pKillFeed.m_dataModel)
	{
		Warning("RkKillFeed already loaded, call unload first!\n");
		return;
	}

	// Create the data binding, this will sync data between rocketui and the game.
	Rml::DataModelConstructor constructor = hudCtx->CreateDataModel("killfeed_model");
	if (!constructor)
	{
		Error("Couldn't create datamodel for killfeed!\n");
		/* Exit */
	}

	// Register KillfeedEntry struct definition
	static bool is_struct_registered = false;
	if (!is_struct_registered)
	{
		if (auto killfeedentry_handle = constructor.RegisterStruct<KillfeedEntry>())
		{
			killfeedentry_handle.RegisterMember("attacker_name", &KillfeedEntry::attackerName);
			killfeedentry_handle.RegisterMember("gun_name", &KillfeedEntry::gunName);
			killfeedentry_handle.RegisterMember("victim_name", &KillfeedEntry::victimName);
			killfeedentry_handle.RegisterMember("headshot", &KillfeedEntry::headshot);
			killfeedentry_handle.RegisterMember("wallbang", &KillfeedEntry::wallbang);
			killfeedentry_handle.RegisterMember("inair", &KillfeedEntry::inair);
			killfeedentry_handle.RegisterMember("killed_you", &KillfeedEntry::killed_you);
			killfeedentry_handle.RegisterMember("you_killed", &KillfeedEntry::you_killed);
			is_struct_registered = true;
		}
	}

	// Register array-type of KillfeedEntry
	static bool arrays_registered = false;
	if (!arrays_registered)
	{
		constructor.RegisterArray<std::deque<KillfeedEntry>>();
		arrays_registered = true;
	}
	// Bind the killfeed entry array
	constructor.Bind("killfeed_entries", &killFeedData.entries);

	pKillFeed.m_dataModel = constructor.GetModelHandle();

	// Load document from file.
	pKillFeed.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud("body", "GAME", "rocketui/hud_killfeed.rml", &kfDocReload);

	if (!pKillFeed.m_pInstance)
	{
		Error("Couldn't create hud_killfeed document!\n");
		/* Exit */
	}

	pKillFeed.ShowPanel(true, false);
}

RkHudKillfeed::RkHudKillfeed(const char *value)
    : m_bVisible(false)
    , m_pInstance(nullptr)
{
	// SetHiddenBits(/* HIDEHUD_MISCSTATUS */ 0);
}

RkHudKillfeed::~RkHudKillfeed() noexcept
{
	// StopListeningForAllEvents();

	UnloadRkKillFeed();
}

void RkHudKillfeed::LevelInit()
{
	LoadRkKillFeed();
}

void RkHudKillfeed::LevelShutdown()
{
	killFeedData.entries.clear();

	UnloadRkKillFeed();
}

void RkHudKillfeed::ShowPanel(bool bShow, bool force)
{
	if (!m_pInstance)
		return;

	if (bShow)
	{
		if (!m_bVisible)
		{
			m_pInstance->Show();
		}
	}
	else
	{
		if (m_bVisible)
		{
			m_pInstance->Hide();
		}
	}
	CheckForOldEntries();
	m_bVisible = bShow;
}

void RkHudKillfeed::SetActive(bool bActive)
{
	ShowPanel(bActive, false);
	// CHudElement::SetActive(bActive);
}

bool RkHudKillfeed::ShouldDraw()
{
	// return cl_drawhud.GetBool() && CHudElement::ShouldDraw();
	return 1;
}

// void RkHudKillfeed::FireGameEvent(IGameEvent *event)
// {
// 	// We only listen for "player_death"
// 	OnPlayerDeath(event);
// }