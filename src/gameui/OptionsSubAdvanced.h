//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef OPTIONS_SUB_ADVANCED_H
#define OPTIONS_SUB_ADVANCED_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>

namespace vgui
{
    class CheckButton;
}

class CContentControlDialog;
//-----------------------------------------------------------------------------
// Purpose: Advanced Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class COptionsSubAdvanced : public vgui2::PropertyPage
{
public:
	COptionsSubAdvanced(vgui2::Panel *parent);
	~COptionsSubAdvanced();

	virtual void OnResetData();
	virtual void OnApplyChanges();

	virtual void OnCommand( const char *command );

    void OnOpenContentControlDialog();

private:
	DECLARE_PANELMAP();
    void OnContentControlClose();

	CContentControlDialog   *m_pContentControlDialog;
    vgui2::CheckButton       *m_pContentCheckButton;

	typedef vgui2::PropertyPage BaseClass;
};

#endif // OPTIONS_SUB_ADVANCED_H
