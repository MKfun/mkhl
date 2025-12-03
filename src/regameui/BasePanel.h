//
// Created by den4ik on 01.12.25.
//

#ifndef BASEPANEL_H
#define BASEPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>

//-----------------------------------------------------------------------------
// Purpose: The panel at the top of the vgui panel hierarchy
//-----------------------------------------------------------------------------
class CBasePanel : public vgui2::Panel
{
public:
	CBasePanel();

	virtual void OnChildAdded(vgui2::VPANEL child);
	virtual void PaintBackground();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

	enum EBackgroundState
	{
		BACKGROUND_NONE,
		BACKGROUND_BLACK,
		BACKGROUND_DESKTOPIMAGE,
		BACKGROUND_LOADING,
		BACKGROUND_LOADINGTRANSITION,
	};

	void SetBackgroundRenderState(EBackgroundState state);

	void RunFrame();

private:
	void DrawBackgroundImage();

	EBackgroundState m_eBackgroundState;

	enum { BACKGROUND_ROWS = 3, BACKGROUND_COLUMNS = 4 };
	struct bimage_t
	{
		int imageID;
		int width, height;
	};
	bimage_t m_ImageID[BACKGROUND_ROWS][BACKGROUND_COLUMNS];
	typedef vgui2::Panel BaseClass;
};

//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
extern CBasePanel *BasePanel();

#endif // BASEPANEL_H