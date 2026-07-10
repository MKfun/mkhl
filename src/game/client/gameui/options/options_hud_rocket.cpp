#include "options_hud_rocket.h"
#include "gameui/options/cvar_text_entry.h"

CRocketHudSettings::CRocketHudSettings(vgui2::Panel *parent, const char *name)
    : BaseClass(parent, name)
{
	ConVarRef scale("rocket_hud_scale");
	m_pRocketEnable = new CCvarCheckButton(this, "RocketEnable", "#BHL_RocketEnable", "rocket_enable");
	// m_pRocketHudScaleSlider = new CCvarSlider(this, "ScaleSlider", "rocket_hud_scale");
	m_pRocketHudScaleSlider = new vgui2::Slider(this, "ScaleSlider");
	m_pRocketHudScaleSlider->SetRange(0, 200);
	m_pRocketHudScaleSlider->SetValue(scale.GetFloat() * 100);
	m_pRocketHudScale = new CCvarTextEntry(this, "RocketHudScale", "rocket_hud_scale", CCvarTextEntry::CvarType::Float);
	m_pRocketKFLinger = new CCvarTextEntry(this, "RocketKillfeedLinger",
	    "rocket_hud_killfeed_linger_time", CCvarTextEntry::CvarType::Int);
	LoadControlSettings("ui/resource/options/OptionsSubRocket.res");
}
void CRocketHudSettings::OnApplyChanges()
{
	m_pRocketEnable->ApplyChanges();
	m_pRocketHudScale->ApplyChanges();
	m_pRocketKFLinger->ApplyChanges();
}
void CRocketHudSettings::OnResetData()
{
	m_pRocketEnable->ResetData();
	m_pRocketHudScale->ResetData();
	m_pRocketHudScaleSlider->SetValue(m_pRocketHudScale->GetValueAsFloat() * 100);
	m_pRocketKFLinger->ResetData();
}
void CRocketHudSettings::OnSliderMoved(KeyValues *kvData)
{
	m_pRocketHudScale->SetValue(m_pRocketHudScaleSlider->GetValue() * 0.01f);
}
void CRocketHudSettings::OnCvarTextChanged(KeyValues *kvData)
{
	m_pRocketHudScaleSlider->SetValue(m_pRocketHudScale->GetValueAsFloat() * 100);
}