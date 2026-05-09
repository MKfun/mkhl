#ifndef KISAKSTRIKE_RKHUD_SPEEDOMETER_H
#define KISAKSTRIKE_RKHUD_SPEEDOMETER_H
#include "rmlui/rkhud_elem_interface.h"
#include <rmlui/rocketuiimpl.h>
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/DataModelHandle.h>
#pragma pop_macro("Assert")
extern ConVar cl_drawhud;

class RkHudSpeedometer : public CRocketHudElem
{
public:
	explicit RkHudSpeedometer(const char *value);
	virtual ~RkHudSpeedometer();
	static RkHudSpeedometer m_Instance;
	// Overrides from CHudElement
	void LevelInit(void);
	virtual void LevelShutdown(void);
	virtual void SetActive(bool bActive);
	virtual bool ShouldDraw(void);
	void ShowPanel(bool bShow, bool force);
	static int m_iSpeed;
	static int m_iJumpSpeed;

	Rml::ElementDocument *m_pInstance;
	Rml::DataModelHandle m_dataModel;
	bool m_bVisible;
};
#endif //KISAKSTRIKE_RKHUD_SPEEDOMETER_H