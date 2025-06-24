//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef OPTIONS_SUB_MOUSE_H
#define OPTIONS_SUB_MOUSE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>

class CCvarNegateCheckButton;
class CKeyToggleCheckButton;
class CCvarToggleCheckButton;
class CCvarSlider;

namespace vgui
{
    class Label;
    class Panel;
}

//-----------------------------------------------------------------------------
// Purpose: Mouse Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubMouse : public vgui2::PropertyPage
{
public:
	COptionsSubMouse(vgui2::Panel *parent);
	~COptionsSubMouse();

	virtual void OnResetData();
	virtual void OnApplyChanges();

protected:
    virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

private:
	DECLARE_PANELMAP();

	void OnControlModified(vgui2::Panel *panel);
    void OnTextChanged(vgui2::Panel *panel);
    void UpdateSensitivityLabel();

	CCvarNegateCheckButton		*m_pReverseMouseCheckBox;
	CKeyToggleCheckButton		*m_pMouseLookCheckBox;
	CCvarToggleCheckButton		*m_pMouseFilterCheckBox;
	CCvarToggleCheckButton		*m_pJoystickCheckBox;
	CKeyToggleCheckButton		*m_pJoystickLookCheckBox;

	CCvarSlider					*m_pMouseSensitivitySlider;
    vgui2::TextEntry             *m_pMouseSensitivityLabel;

	CCvarToggleCheckButton		*m_pAutoAimCheckBox;

	typedef vgui2::PropertyPage BaseClass;
};



#endif // OPTIONS_SUB_MOUSE_H
