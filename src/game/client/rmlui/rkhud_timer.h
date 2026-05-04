#ifndef KISAKSTRIKE_RKHUD_ROUNDTIMER_H
#define KISAKSTRIKE_RKHUD_ROUNDTIMER_H
#include <rmlui/rocketuiimpl.h>
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/DataModelHandle.h>
#pragma pop_macro("Assert")
extern ConVar cl_drawhud;

class RkHudRoundTimer
{
public:
	explicit RkHudRoundTimer(const char *value);
	virtual ~RkHudRoundTimer();
	static RkHudRoundTimer m_Instance;
	// Overrides from CHudElement
	void LevelInit(void);
	virtual void LevelShutdown(void);
	virtual void SetActive(bool bActive);
	virtual bool ShouldDraw(void);
	void ShowPanel(bool bShow, bool force);
	static int m_iRemainingTime;
	// Struct layout for data-binding model.
	static struct RoundTimerData
	{
		int MinutesLeft;
		int SecondsLeft;
	} roundTimerData;

	Rml::ElementDocument *m_pInstance;
	Rml::DataModelHandle m_dataModel;
	bool m_bVisible;
};
#endif //KISAKSTRIKE_RKHUD_ROUNDTIMER_H