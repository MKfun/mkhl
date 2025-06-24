//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef COMMANDCHECKBUTTON_H
#define COMMANDCHECKBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/CheckButton.h>

class CCommandCheckButton : public vgui2::CheckButton
{
public:
	CCommandCheckButton( vgui2::Panel *parent, const char *panelName, const char *text, const char *downcmd, const char *upcmd );
	~CCommandCheckButton();

//	virtual void	OnCheckButtonChecked(vgui2::Panel *panel);
	virtual void	SetSelected( bool state );
private:
	typedef vgui2::CheckButton BaseClass;

	char			*m_pszDown;
	char			*m_pszUp;
};
#endif // COMMANDCHECKBUTTON_H
