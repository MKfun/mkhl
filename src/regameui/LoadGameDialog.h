//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef LOADGAMEDIALOG_H
#define LOADGAMEDIALOG_H
#include "GameConsoleDialog.h"
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>

namespace vgui2
{
class ListPanel;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CLoadGameDialog : public vgui2::Frame
{
public:
	static const int SAVEGAME_MAPNAME_LEN;
	static const int SAVEGAME_COMMENT_LEN;
	static const int SAVEGAME_ELAPSED_LEN;

	CLoadGameDialog(vgui2::Panel *parent);
	~CLoadGameDialog();

	virtual void OnCommand(const char *command);
	virtual void OnClose();

protected:
	void CreateSavedGamesList(void);
	void ScanSavedGames(void);
	bool ParseSaveData(char const *pszFileName, char const *pszShortName, KeyValues *kv);

	vgui2::ListPanel *m_pGameList;
	typedef vgui2::Frame BaseClass;
};

#endif // LOADGAMEDIALOG_H