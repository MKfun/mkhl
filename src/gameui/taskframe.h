//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef TASKFRAME_H
#define TASKFRAME_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include "Taskbar.h"
#include <vgui/VGUI.h>
#include <vgui/ILocalize.h>

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTaskFrame : public vgui2::Frame
{
public:
	CTaskFrame(vgui2::Panel *parent, const char *panelName, bool showTaskbarIcon=true): vgui2::Frame(parent,panelName,showTaskbarIcon) {}
	~CTaskFrame(){}

	virtual void SetTitle(const char *title, bool surfaceTitle)
	{
		BaseClass::SetTitle(title,surfaceTitle);

		if (g_pTaskbar)
		{
			wchar_t *wTitle;
			wchar_t w_szTitle[1024];

			wTitle = vgui2::localize()->Find(title);

			if(!wTitle)
			{
				vgui2::localize()->ConvertANSIToUnicode(title,w_szTitle,sizeof(w_szTitle));
				wTitle = w_szTitle;
			}

			g_pTaskbar->SetTitle(GetVPanel(), wTitle);
		}
	}





	typedef vgui2::Frame BaseClass;
};

#endif // TASKFRAME_H
