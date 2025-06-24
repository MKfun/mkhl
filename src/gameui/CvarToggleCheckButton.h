//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef CVARTOGGLECHECKBUTTON_H
#define CVARTOGGLECHECKBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/CheckButton.h>

class CCvarToggleCheckButton : public vgui2::CheckButton
{
public:
	CCvarToggleCheckButton( vgui2::Panel *parent, const char *panelName, const char *text,
		char const *cvarname );
	~CCvarToggleCheckButton();

	virtual void	SetSelected( bool state );

	virtual void	Paint();

	void			Reset();
	void			ApplyChanges();
	bool			HasBeenModified();

private:
	typedef vgui2::CheckButton BaseClass;

	DECLARE_PANELMAP();
	void			OnButtonChecked();

	char			*m_pszCvarName;
	bool			m_bStartValue;
};

#endif // CVARTOGGLECHECKBUTTON_H
