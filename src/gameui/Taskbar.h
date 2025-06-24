//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef TASKBAR_H
#define TASKBAR_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "utlvector.h"

//MODDD - WHY NOT??
#include "public/vgui/IScheme.h"
#include "public/vgui/IClientPanel.h"


class CTaskButton;
class CBackgroundMenuButton;
class CGameMenu;

//-----------------------------------------------------------------------------
// Purpose: Win9X-type taskbar for in the game
//-----------------------------------------------------------------------------
class CTaskbar : public vgui2::EditablePanel
{
public:

	struct vgui2::PanelMessageMap* GetMessageMap();

	CTaskbar(vgui2::Panel *parent, const char *panelName);
	~CTaskbar();

	void ApplySchemeSettings(vgui2::IScheme* pScheme);
	void OnCommand(char* command);
	void PerformLayout();

	void OnActivateModule(int moduleIndex);

	void ActivateGameUI();




	// update the taskbar a frame
	void RunFrame();

	// sets the title of a task
	void SetTitle(vgui2::VPANEL panel, const wchar_t *title);

	// adds a new task to the list
	void AddTask(vgui2::VPANEL panel);

	// handles gameUI being shown
	void OnGameUIActivated();

	// game dialogs
	void OnOpenNewGameDialog();
	void OnOpenLoadGameDialog();
	void OnOpenSaveGameDialog();
	void OnOpenOptionsDialog();
	void OnOpenServerBrowser();
	void OnOpenDemoDialog();
	void OnOpenCreateMultiplayerGameDialog();
	void OnOpenQuitConfirmationDialog();
	void OnOpenChangeGameDialog();
	void OnOpenPlayerListDialog();

	void PositionDialog(vgui2::PHandle dlg);

private:
	// menu manipulation
	void CreatePlatformMenu();
	void CreateGameMenu();
	void UpdateGameMenus();
	void UpdateTaskButtons();
	CGameMenu *RecursiveLoadGameMenu(KeyValues *datafile);

	CTaskButton *FindTask(vgui2::VPANEL panel);

	virtual void OnCommand(const char *command);
	//virtual void PerformLayout();
	//virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);
	//virtual void OnActivateModule(int moduleIndex);


	DECLARE_PANELMAP();

	// menu buttons
	CBackgroundMenuButton *m_pGameMenuButton;
	CBackgroundMenuButton *m_pPlatformMenuButton;
	CGameMenu *m_pPlatformMenu;
	CGameMenu *m_pGameMenu;
	bool m_bShowPlatformMenu;
	bool m_bPlatformMenuInitialized;
	int m_iGameMenuInset;

	// base dialogs
	vgui2::DHANDLE<vgui2::Frame> m_hNewGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hLoadGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hSaveGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hOptionsDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCreateMultiplayerGameDialog;
	//vgui2::DHANDLE<vgui2::Frame> m_hDemoPlayerDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hChangeGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hPlayerListDialog;

	// list of all the tasks
	CUtlVector<CTaskButton *> g_Tasks;

	typedef vgui2::Panel BaseClass;
};

extern CTaskbar *g_pTaskbar;


#endif // TASKBAR_H
