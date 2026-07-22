//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef CREATEMULTIPLAYERGAMEDIALOG_H
#define CREATEMULTIPLAYERGAMEDIALOG_H
#ifdef _WIN32
#pragma once
#endif

namespace vgui2
{
	class PropertyDialog;
	class PropertySheet;
}

#include <vgui_controls/PropertyDialog.h>

class CCreateMultiplayerGameServerPage;
class CCreateMultiplayerGameBotPage;

//-----------------------------------------------------------------------------
// Purpose: dialog for launching a listenserver
//-----------------------------------------------------------------------------
class CCreateMultiplayerGameDialog : public vgui2::PropertyDialog
{
public:
	CCreateMultiplayerGameDialog(vgui2::Panel *parent);
	~CCreateMultiplayerGameDialog();

protected:
	virtual bool OnOK(bool b1) override;
	virtual void OnClose();
	virtual void SetTitle(const char *title, bool surfaceTitle);

private:
	typedef vgui2::PropertyDialog BaseClass;

	CCreateMultiplayerGameServerPage *m_pServerPage;
	CCreateMultiplayerGameBotPage *m_pBotPage;

	// for loading/saving bot config
	KeyValues *m_pBotSavedData;
};

#endif // CREATEMULTIPLAYERGAMEDIALOG_H
