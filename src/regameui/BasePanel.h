//
// Created by den4ik on 01.12.25.
//

#ifndef BASEPANEL_H
#define BASEPANEL_H
#include "GameMenuBar.h"
#include "LoadGameDialog.h"
#include "SaveGameDialog.h"
#include "createmultiplayer/CreateMultiplayerGameDialog.h"
#include "options/optionsdialog.h"
#include "serverconnectpanel.h"
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Panel.h>
#include <vgui_controls/EditablePanel.h>
#include "vgui_controls/Button.h"
#include "GameMenu.h"
class CBackgroundMenuButton;
class CNewGameDialog;
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
	void OnGameUIActivated();
	virtual void RunFrame();
	virtual void PerformLayout();

private:
    void DrawBackgroundImage();
	void OnOpenOptionsDialog(int tab = 0);
	EBackgroundState m_eBackgroundState;

	struct bimage_t
	{
		int imageID;
		int x, y;
		int width, height;
		bool scaled;
		bool fit;
	};
	CUtlVector<bimage_t> m_ImageID;
	int m_iBaseResX, m_iBaseResY;

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
	CGameMenuBar *m_pMenuBar;
	CGameMenuItem *m_pGameMenuButton;

	vgui2::DHANDLE<COptionsDialog> m_hOptionsDialog;
	vgui2::DHANDLE<ServerConnectPanel> m_hServerConnectDialog;
	vgui2::DHANDLE<CCreateMultiplayerGameDialog> m_hCreateMultiplayerGameDialog;
	vgui2::DHANDLE<CNewGameDialog> m_hNewGameDialog;
	vgui2::DHANDLE<CLoadGameDialog> m_hLoadGameDialog;
	vgui2::DHANDLE<CSaveGameDialog> m_hSaveGameDialog;
	// misc
	void PositionDialog(vgui2::PHandle dlg);
	void SetupThatFrickinPanel();

	int m_iLoadingImageID;
	bool m_bLevelLoading;
	bool m_bEverActivated;
	bool m_bFadingInMenus;
	bool m_bInitialLoading;
	float m_flFadeMenuStartTime;
	float m_flFadeMenuEndTime;
	bool m_bRenderingBackgroundTransition;
	float m_flTransitionStartTime;
	float m_flTransitionEndTime;
	bool m_bHaveDarkenedBackground;
	bool m_bHaveDarkenedTitleText;
	bool m_bForceTitleTextUpdate;
	float m_flFrameFadeInTime;
	Color m_BackdropColor;
};

//-----------------------------------------------------------------------------
// Purpose: singleton accessor
//-----------------------------------------------------------------------------
extern CBasePanel *BasePanel();

#endif // BASEPANEL_H
