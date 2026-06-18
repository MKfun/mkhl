//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#include "CreateMultiplayerGameDialog.h"
#include "CreateMultiplayerGameServerPage.h"
#include "CreateMultiplayerGameGameplayPage.h"
// #include "CreateMultiplayerGameBotPage.h"

#include "../EngineInterface.h"
#include "../ModInfo.h"
#include "convar.h"

#include <stdio.h>

using namespace vgui2;
#include <vgui/ILocalize.h>

#include "FileSystem.h"
#include <KeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

// CS
#include <vgui_controls/RadioButton.h>

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameDialog::CCreateMultiplayerGameDialog(vgui2::Panel *parent)
    : PropertyDialog(parent, "CreateMultiplayerGameDialog")
{
	SetSize(424, 420);

	SetTitle("#GameUI_CreateServer", true);
	SetOKButtonText("#GameUI_Start");

	m_pServerPage = new CCreateMultiplayerGameServerPage(this, "ServerPage");
	AddPage(m_pServerPage, "#GameUI_Server");
	AddPage(new CCreateMultiplayerGameGameplayPage(this, "GameplayPage"), "#GameUI_Game");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameDialog::~CCreateMultiplayerGameDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: Overrides the base class so it can setup the taskbar title properly
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameDialog::SetTitle(const char *title, bool surfaceTitle)
{
}

//-----------------------------------------------------------------------------
// Purpose: runs the server when the OK button is pressed
//-----------------------------------------------------------------------------
bool CCreateMultiplayerGameDialog::OnOK(bool b1)
{

	// get these values from m_pServerPage and store them temporarily
	char szMapName[64], szHostName[64], szPassword[64];
	strncpy(szMapName, m_pServerPage->GetMapName(), sizeof(szMapName));
	strncpy(szHostName, m_pServerPage->GetHostName(), sizeof(szHostName));
	strncpy(szPassword, m_pServerPage->GetPassword(), sizeof(szPassword));

	char szMapCommand[1024];

	{
		// create the command to execute
		sprintf(szMapCommand, "disconnect\nsv_lan 1\nsetmaster enable\nmaxplayers %i\nsv_password \"%s\"\nhostname \"%s\"\nmap %s\n",
		    m_pServerPage->GetMaxPlayers(),
		    szPassword,
		    szHostName,
		    szMapName);

		// exec
		gEngfuncs.pfnClientCmd(szMapCommand);
	}
	return BaseClass::OnOK(b1);
}

//-----------------------------------------------------------------------------
// Purpose: deletes the dialog when it gets closed
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameDialog::OnClose()
{
	MarkForDeletion();
	BaseClass::OnClose();
}