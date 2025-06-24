//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef BACKGROUNDMENUBUTTON_H
#define BACKGROUNDMENUBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Button.h>

//-----------------------------------------------------------------------------
// Purpose: Baseclass for the left and right ingame menus that lay on the background
//-----------------------------------------------------------------------------
class CBackgroundMenuButton : public vgui2::Button
{
public:
	CBackgroundMenuButton(vgui2::Panel *parent, const char *name);
	~CBackgroundMenuButton();

	virtual void OnCommand(const char *command);

	void SetImages(vgui2::IImage *image, vgui2::IImage *mouseoverImage)
	{
		m_pImage = image;
		m_pMouseOverImage = mouseoverImage;
		SetArmed(IsArmed());
	}

	virtual void SetArmed(bool state)
	{
		if (state)
		{
			SetImageAtIndex(0, m_pMouseOverImage, 0);
		}
		else
		{
			SetImageAtIndex(0, m_pImage, 0);
		}

		BaseClass::SetArmed(state);
	}

protected:
	vgui2::Menu *RecursiveLoadGameMenu(KeyValues *datafile);
	vgui2::Menu *m_pMenu;

	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

private:
	vgui2::IImage *m_pImage, *m_pMouseOverImage;
	typedef vgui2::Button BaseClass;
};


#endif // BACKGROUNDMENUBUTTON_H
