//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef GAMEMENUBUTTON_H
#define GAMEMENUBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/MenuButton.h>
#include <vgui_controls/PHandle.h>

namespace vgui
{
class Frame;
}

//-----------------------------------------------------------------------------
// Purpose: Button on the taskbar that has all the game-specific options
//-----------------------------------------------------------------------------
class CGameMenuButton : public vgui2::MenuButton
{
public:
	CGameMenuButton( vgui2::Panel *parent, const char *panelName, const char *text );
	~CGameMenuButton();

	void OnOpenNewGameDialog();
	void OnOpenLoadGameDialog();
	void OnOpenSaveGameDialog();
	void OnOpenMultiplayerDialog();
	void OnOpenMultiplayerAdvancedDialog();
	void OnOpenOptionsDialog();
	void OnOpenContentControlDialog();
	void OnOpenServerBrowser();
	void OnOpenCreateMultiplayerGameDialog();

protected:
	virtual void OnCommand(const char *command);
	virtual void PerformLayout();

private:
	vgui2::Menu *RecursiveLoadGameMenu(KeyValues *datafile);

	typedef vgui2::MenuButton BaseClass;
	vgui2::Menu *m_pMenu;

	void				PositionDialog( vgui2::PHandle dlg );

	vgui2::DHANDLE<vgui2::Frame> m_hNewGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hLoadGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hSaveGameDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hOptionsDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hMultiplayerCustomizeDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hMultiplayerAdvancedDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hContentControlDialog;
	vgui2::DHANDLE<vgui2::Frame> m_hCreateMultiplayerGameDialog;
};

// Singleton accessor
extern CGameMenuButton *g_pGameMenuButton;

#endif // GAMEMENUBUTTON_H
