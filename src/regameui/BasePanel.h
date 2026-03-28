//
// Created by den4ik on 01.12.25.
//

#ifndef BASEPANEL_H
#define BASEPANEL_H
#include "options/optionsdialog.h"
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <vgui_controls/EditablePanel.h>
#include "vgui_controls/Button.h"
#include "GameMenu.h"
class CBackgroundMenuButton;
typedef struct {int x; int y;} coord;
//-----------------------------------------------------------------------------
// Purpose: The panel at the top of the vgui panel hierarchy
//-----------------------------------------------------------------------------
class CBasePanel : public vgui2::EditablePanel
{
public:
	CBasePanel();

	virtual void OnChildAdded(vgui2::VPANEL child);
	virtual void PaintBackground();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
    virtual void OnCommand(const char *command);
	enum EBackgroundState
	{
		BACKGROUND_NONE,
		BACKGROUND_BLACK,
		BACKGROUND_DESKTOPIMAGE,
		BACKGROUND_LOADING,
		BACKGROUND_LOADINGTRANSITION,
	};

	void SetBackgroundRenderState(EBackgroundState state);

	virtual void RunFrame();
	virtual void PerformLayout();

private:
    void DrawBackgroundImage();
    void OnOpenOptionsDialog();
    EBackgroundState m_eBackgroundState;

	enum { BACKGROUND_ROWS = 3, BACKGROUND_COLUMNS = 4 };
	struct bimage_t
	{
		int imageID;
		int width, height;
	};
	bimage_t m_ImageID[BACKGROUND_ROWS][BACKGROUND_COLUMNS];
	typedef vgui2::Panel BaseClass;
	CGameMenu *RecursiveLoadGameMenu(KeyValues *datafile);
	void CreateGameMenu();
	CUtlVector< coord > m_iGameTitlePos;
//	CUtlVector< CBackgroundMenuButton * >m_pGameMenuButtons;
	CUtlVector< CBackgroundMenuButton * >m_pGameMenuButtons;
	int m_iGameMenuInset;
	coord m_iGameMenuPos;
	void UpdateGameMenus();
    //Controls
    CGameMenu *m_pGameMenu;
    vgui2::MenuBar *m_pMenuBar;
    CGameMenuItem *m_pGameMenuButton;

    COptionsDialog *m_hOptionsDialog;

    // misc
    void PositionDialog(vgui2::PHandle dlg);
};

//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
extern CBasePanel *BasePanel();

#endif // BASEPANEL_H
