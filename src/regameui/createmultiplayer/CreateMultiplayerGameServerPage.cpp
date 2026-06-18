//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#include "CreateMultiplayerGameServerPage.h"
#include "convar.h"

using namespace vgui2;

#include <KeyValues.h>
#include <vgui_controls/ListPanel.h>
#include <vgui_controls/RadioButton.h>
#include "FileSystem.h"
#include "../EngineInterface.h"

#include "../ModInfo.h"

// for SRC
#include <vstdlib/random.h>
//#include "Random.h"

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

#define RANDOM_MAP "< Random Map >"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::CCreateMultiplayerGameServerPage(vgui2::Panel *parent, const char *name)
    : PropertyPage(parent, name)
{
	// we can use this if we decide we want to put "listen server" at the end of the game name
	//	static char szHostName[256];
	//	_snprintf( szHostName, sizeof( szHostName ) - 1, "%s %s", ModInfo().GetGameDescription(), "Listen Server" );
	//	szHostName[sizeof( szHostName ) - 1] = '\0';

	m_pMapList = new ListPanel(this, "MapList");

	LoadControlSettings("Resource/CreateMultiplayerGameServerPage.res");

	m_pMapList->AddColumnHeader(0, "mapname", "#GameUI_Map", m_pMapList->GetWide(), 0);
	LoadMapList();

	m_szMapName[0] = 0;
	m_szHostName[0] = 0;
	m_szPassword[0] = 0;
	m_iMaxPlayers = gEngfuncs.GetMaxClients();

	// make sure this will be a multiplayer game
	if (m_iMaxPlayers <= 1)
	{
		m_iMaxPlayers = 20; // this was the default for the old launcher
	}

	// initialize hostname
	SetControlString("ServerNameEdit", ModInfo().GetGameName()); //GetGameDescription()); //szHostName);

	// initialize password
	//	SetControlString("PasswordEdit", gEngfuncs.pfnGetCvarString("sv_password"));
	ConVarRef var("sv_password");
	if (var.IsValid())
	{
		SetControlString("PasswordEdit", var.GetString());
	}

	//	int maxPlayersEdit = atoi( GetControlString( "MaxPlayersEdit", "-1" ) );
	//	if ( maxPlayersEdit <= 1 )
	{
		// initialize maxplayers
		char szBuffer[4];
		_snprintf(szBuffer, sizeof(szBuffer) - 1, "%d", m_iMaxPlayers);
		szBuffer[sizeof(szBuffer) - 1] = '\0';
		SetControlString("MaxPlayersEdit", szBuffer);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CCreateMultiplayerGameServerPage::~CCreateMultiplayerGameServerPage()
{
}

//-----------------------------------------------------------------------------
// Purpose: called to get the info from the dialog
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::OnApplyChanges()
{
	strncpy(m_szHostName, GetControlString("ServerNameEdit", "Half-Life"), DATA_STR_LENGTH);
	strncpy(m_szPassword, GetControlString("PasswordEdit", ""), DATA_STR_LENGTH);
	m_iMaxPlayers = atoi(GetControlString("MaxPlayersEdit", "8"));

	int selectedItemID = m_pMapList->GetSelectedItem(0);
	if (selectedItemID >= 0)
	{
		KeyValues *kv = m_pMapList->GetItem(selectedItemID);
		strncpy(m_szMapName, kv->GetString("mapname", ""), DATA_STR_LENGTH);
	}
}

void CCreateMultiplayerGameServerPage::LoadMaps(const char *pszPathID)
{
	FileFindHandle_t findHandle = NULL;

	KeyValues *hiddenMaps = ModInfo().GetHiddenMaps();

	const char *pszFilename = g_pFullFileSystem->FindFirst("maps/*.bsp", &findHandle, pszPathID);
	while (pszFilename)
	{
		char mapname[256];
		char *ext, *str;

		// FindFirst ignores the pszPathID, so check it here
		// TODO: this doesn't find maps in fallback dirs
		_snprintf(mapname, sizeof(mapname), "maps/%s", pszFilename);
		if (!g_pFullFileSystem->FileExists(mapname))
		{
			goto nextFile;
		}

		// remove the text 'maps/' and '.bsp' from the file name to get the map name

		str = Q_strstr(pszFilename, "maps");
		if (str)
		{
			strncpy(mapname, str + 5, sizeof(mapname) - 1); // maps + \\ = 5
		}
		else
		{
			strncpy(mapname, pszFilename, sizeof(mapname) - 1);
		}
		ext = Q_strstr(mapname, ".bsp");
		if (ext)
		{
			*ext = 0;
		}

		//!! hack: strip out single player HL maps
		// this needs to be specified in a seperate file
		// add to the map list
		m_pMapList->AddItem(new KeyValues("data", "mapname", mapname), 0, false, true);

		// get the next file
	nextFile:
		pszFilename = g_pFullFileSystem->FindNext(findHandle);
	}
	g_pFullFileSystem->FindClose(findHandle);
}

//-----------------------------------------------------------------------------
// Purpose: loads the list of available maps into the map list
//-----------------------------------------------------------------------------
void CCreateMultiplayerGameServerPage::LoadMapList()
{
	// clear the current list (if any)
	m_pMapList->DeleteAllItems();

	KeyValues *hiddenMaps = ModInfo().GetHiddenMaps();
	// add special "name" to represent loading a randomly selected map
	m_pMapList->AddItem(new KeyValues("data", "mapname", RANDOM_MAP), 0, false, false);

	// iterate the filesystem getting the list of all the files
	// UNDONE: steam wants this done in a special way, need to support that
	FileFindHandle_t findHandle = NULL;
	const char *pathID = "GAME";
	if (!stricmp(ModInfo().GetGameName(), "Half-Life"))
	{
		pathID = NULL; // hl is the base dir
	}

	const char *filename = g_pFullFileSystem->FindFirst("maps/*.bsp", &findHandle);
	while (filename)
	{
		// remove the text 'maps/' and '.bsp' from the file name to get the map name
		char mapname[256];

		char *str = (char *)strstr(filename, "maps");
		if (str)
		{
			strncpy(mapname, str + 5, sizeof(mapname) - 1); // maps + \\ = 5
		}
		else
		{
			strncpy(mapname, filename, sizeof(mapname) - 1);
		}
		str = strstr(mapname, ".bsp");
		if (str)
		{
			*str = 0;
		}
		if ((mapname[0] == 'c' || mapname[0] == 't') && mapname[2] == 'a' && mapname[1] >= '0' && mapname[1] <= '5')
		{
			goto nextFile;
		}

		// strip out maps that shouldn't be displayed
		if (hiddenMaps)
		{
			if (hiddenMaps->GetInt(mapname, 0))
			{
				goto nextFile;
			}
		}

		// add to the map list
		m_pMapList->AddItem(new KeyValues("data", "mapname", mapname), 0, false, false);

		// get the next file
	nextFile:
		filename = g_pFullFileSystem->FindNext(findHandle);
	}
	g_pFullFileSystem->FindClose(findHandle);

	// set the first item to be selected
	if (m_pMapList->GetItemCount() > 0)
	{
		m_pMapList->SetSingleSelectedItem(m_pMapList->GetItemIDFromRow(0));
	}
}

const char *CCreateMultiplayerGameServerPage::GetMapName()
{
	int count = m_pMapList->GetItemCount();

	// if there is only one entry it's the special "select random map" entry
	if (count <= 1)
		return NULL;

	const char *mapname = m_pMapList->GetItem(m_pMapList->GetSelectedItem(0))->GetString("mapname");
	if (!strcmp(mapname, RANDOM_MAP))
	{
		int which = RandomInt(1, count - 1);
		mapname = m_pMapList->GetItem(which)->GetString("mapname");
	}

	return mapname;
}

int CCreateMultiplayerGameServerPage::GetMaxPlayers()
{
	return atoi(GetControlString("MaxPlayersEdit", "8"));
}

const char *CCreateMultiplayerGameServerPage::GetPassword()
{
	return GetControlString("PasswordEdit", "");
}

const char *CCreateMultiplayerGameServerPage::GetHostName()
{
	return GetControlString("ServerNameEdit", "Half-Life");
}