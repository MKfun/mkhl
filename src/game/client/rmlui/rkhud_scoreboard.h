#ifndef RKHUD_SCOREBOARD_H
#define RKHUD_SCOREBOARD_H

#include "rmlui/rkhud_elem_interface.h"
#include <rmlui/rocketuiimpl.h>

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/DataModelHandle.h>
#pragma pop_macro("Assert")

extern ConVar cl_drawhud;

class RkHudScoreboard : public CRocketHudElem
{
public:
	explicit RkHudScoreboard(const char *value);
	virtual ~RkHudScoreboard();
	static RkHudScoreboard m_Instance;

	// Overrides from CRocketHudElem
	void LevelInit(void) override;
	virtual void LevelShutdown(void) override;
	virtual void SetActive(bool bActive) override;
	virtual bool ShouldDraw(void) override;
	void ShowPanel(bool bShow, bool force) override;
	virtual void Update(void) override;

	Rml::ElementDocument *m_pInstance = nullptr;
	Rml::DataModelHandle m_dataModel;
	bool m_bVisible = false;
};

#endif // RKHUD_SCOREBOARD_H
