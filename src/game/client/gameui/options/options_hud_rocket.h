#ifndef OPTIONS_HUD_ROCKET_H
#define OPTIONS_HUD_ROCKET_H

#include "gameui/options/cvar_text_entry.h"
#include "gameui/options/cvar_check_button.h"
#include "gameui/options/cvar_slider.h"
#include "vgui_controls/PropertyPage.h"
class CCvarTextEntry;
class CRocketHudSettings : public vgui2::PropertyPage
{
	DECLARE_CLASS_SIMPLE(CRocketHudSettings, vgui2::PropertyPage);

public:
	CRocketHudSettings(vgui2::Panel *parent, const char *name);
	void OnResetData() override;
	void OnApplyChanges() override;

private:
	CCvarCheckButton *m_pRocketEnable;
	CCvarTextEntry *m_pRocketKFLinger;
	vgui2::Slider *m_pRocketHudScaleSlider;
	CCvarTextEntry *m_pRocketHudScale;

	MESSAGE_FUNC_PARAMS(OnSliderMoved, "SliderMoved", kv);
	MESSAGE_FUNC_PARAMS(OnCvarTextChanged, "CvarTextChanged", kv);
};

#endif // OPTIONS_HUD_ROCKET_H
