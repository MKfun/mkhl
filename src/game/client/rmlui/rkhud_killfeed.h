#ifndef KISAKSTRIKE_RKHUD_KILLFEED_H
#define KISAKSTRIKE_RKHUD_KILLFEED_H

#include "rocketuiimpl.h"

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/DataModelHandle.h>
#pragma pop_macro("Assert")

extern ConVar cl_drawhud;

class RkHudKillfeed
{
public:
	explicit RkHudKillfeed(const char *value);
	virtual ~RkHudKillfeed();
	static RkHudKillfeed m_Instance;
	// Overrides from CHudElement
	void LevelInit(void);
	virtual void LevelShutdown(void);
	virtual void SetActive(bool bActive);
	virtual bool ShouldDraw(void);
	void ShowPanel(bool bShow, bool force);
	void OnPlayerDeath(int killerId, int victimId, const char *killedwith, int killrarity);
	// CGameEventListener THERE IS NO GAMEEVENTS :crymore:
	// virtual void FireGameEvent(IGameEvent *event);

	Rml::ElementDocument *m_pInstance;
	bool m_bVisible;
	Rml::DataModelHandle m_dataModel;

private:
	void CheckForOldEntries();
	// void OnPlayerDeath(IGameEvent *event);
};

#endif //KISAKSTRIKE_RKHUD_KILLFEED_H