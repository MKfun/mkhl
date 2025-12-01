//
// Created by den4ik on 01.12.25.
//
#ifdef WIN32
#if !defined( _X360 )
#include <windows.h>
#endif
#include <io.h>
#include <direct.h>
#elif defined( __GNUC__ )
#include <sys/time.h>
#else
#error
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <tier0/dbg.h>

#ifdef SendMessage
#undef SendMessage
#endif

#include "FileSystem.h"
#include "GameUI_Interface.h"
//#include "Sys_Utils.h"
#include "string.h"
#include "tier0/icommandline.h"

#include "EngineInterface.h"
#include <IEngineVGui.h>
#include "IBaseUI.h"

#include "BasePanel.h"

#include <vgui/Cursor.h>
#include <KeyValues.h>
#include <vgui/ILocalize.h>
#include <vgui/IPanel.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/ISystem.h>
#include <vgui/ISurface.h>
#include <vgui_controls/Menu.h>
#include <vgui_controls/PHandle.h>

#include "tier0/vcrmode.h"

#include <tier0/memdbgon.h>

IGameUIFuncs *gameuifuncs = NULL;
IEngineVGui *enginevguifuncs = NULL;
vgui2::ISurface *enginesurfacefuncs = NULL;
IBaseUI *baseuifuncs = NULL;
//IFriendsUser *g_pFriendsUser = NULL;

#include "tier0/dbg.h"
#include "cl_dll/IGameClientExports.h"
#include "IGameUI.h"
#include "IGameUIFuncs.h"
#include "icvar.h"
#include "VGuiSystemModuleLoader.h"
//#include "engine/.h"
cl_enginefunc_t *engine = NULL;
ICvar *cvar = NULL;

static CBasePanel *staticPanel = NULL;

class CGameUI;
CGameUI *g_pGameUI = NULL;


class CLoadingDialog;
vgui2::DHANDLE<CLoadingDialog> g_hLoadingDialog;

static CGameUI g_GameUI;

static int g_hMutex = NULL;
static int g_hWaitMutex = NULL;

static IGameClientExports *g_pGameClientExports = NULL;

IGameClientExports *GameClientExports()
{
	return g_pGameClientExports;
}

CGameUI &GameUI()
{
	return g_GameUI;
}

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameUI, IGameUI, GAMEUI_INTERFACE_VERSION_GS, g_GameUI);

CGameUI::CGameUI()
{
	g_pGameUI = this;
	m_bTryingToLoadTracker = false;
	m_iGameIP = 0;
	m_iGamePort = 0;
	m_flProgressStartTime = 0.0f;
	m_pszCurrentProgressType = "";
	m_bActivatedUI = false;
}
CGameUI::~CGameUI()
{
	g_pGameUI = NULL;
}
void CGameUI::Initialize( CreateInterfaceFn *factories, int count )
{
	CreateInterfaceFn factory = factories[ 0 ];
	CreateInterfaceFn fileSystemFactory = factories[ 0 ];
	CreateInterfaceFn vguiFactory = factories[ 0 ];
	CreateInterfaceFn engineFactory = factories[ 0 ];
	CreateInterfaceFn clientFactory = factories[ 0 ];


//	enginesound = (IEngineSound *)factory(IENGINESOUND_CLIENT_INTERFACE_VERSION, NULL);
	cvar		= (ICvar *)factory( VENGINE_CVAR_INTERFACE_VERSION, NULL );
//	engine		= (cl_enginefunc_t *)factory( VENGINE_CLIENT_INTERFACE_VERSION, NULL );

	m_FactoryList[ 0 ] = Sys_GetFactoryThis();
	m_FactoryList[ 1 ] = factory;
	m_iNumFactories = count;

	vgui2::VGui_InitInterfacesList( "GameUI", m_FactoryList, 2 );

	// load localization file
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/gameui_english.txt");

	// load localization file for kb_act.lst
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/valve_english.txt");

	// setup base panel
	staticPanel = new CBasePanel();
	staticPanel->SetBounds(0, 0, 400, 300);
	staticPanel->SetPaintBorderEnabled(false);
	staticPanel->SetPaintBackgroundEnabled(true);
	staticPanel->SetPaintEnabled(false);
	staticPanel->SetVisible( true );
	staticPanel->SetMouseInputEnabled( false );
	staticPanel->SetKeyBoardInputEnabled( false );

	enginevguifuncs = (IEngineVGui * )engineFactory( VENGINE_VGUI_VERSION, NULL);
	if(enginevguifuncs)
	{
		vgui2::VPANEL rootpanel = enginevguifuncs->GetPanel(PANEL_GAMEUIDLL);
		staticPanel->SetParent(rootpanel);
	}

	gameuifuncs = (IGameUIFuncs * )engineFactory( IGAMEUIFUNCS_NAME, NULL );
	enginesurfacefuncs = (vgui2::ISurface * )engineFactory(VGUI_SURFACE_INTERFACE_VERSION_GS,NULL);
	baseuifuncs = (IBaseUI *)engineFactory( BASEUI_INTERFACE_VERSION, NULL);
	if (clientFactory)
	{
		g_pGameClientExports = (IGameClientExports *)clientFactory(GAMECLIENTEXPORTS_INTERFACE_VERSION, NULL);
	}
}
int __stdcall SendShutdownMsgFunc(int hwnd, int lparam)
{
//	Sys_PostMessage(hwnd, Sys_RegisterWindowMessage("ShutdownValvePlatform"), 0, 1);
	return 1;
}
void CGameUI::Start(cl_enginefunc_t *engineFuncs, int interfaceVersion, void *system)
{
//	ModInfo().LoadCurrentGameInfo();


	// Determine Tracker location.
	// ...If running with Steam, Tracker is in a well defined location relative to the game dir.  Use it if there.
	// ...Otherwise get the tracker location from the registry key
	if (FindPlatformDirectory(m_szPlatformDir, sizeof(m_szPlatformDir)))
	{
		// add the tracker directory to the search path
		// add localized version first if we're not in english
		char language[128];
		if (vgui2::system()->GetRegistryString("HKEY_LOCAL_MACHINE\\Software\\Valve\\Steam\\Language", language, sizeof(language)))
		{
			if (strlen(language) > 0 && stricmp(language, "english"))
			{
				char path[256];
				sprintf(path, "platform_%s", language);
				g_pFullFileSystem->AddSearchPath(path, "PLATFORM");
			}
		}
		g_pFullFileSystem->AddSearchPath("platform", "PLATFORM");

		// setup config file directory
		char szConfigDir[512];

		strcpy(szConfigDir, m_szPlatformDir);
		strcat(szConfigDir, "config");

		/*
		// make sure the path exists
		_finddata_t findData;
		long findHandle = _findfirst(steamPath, &findData);
		if (steamPath && findHandle != -1)
		{
			// put the config dir directly under steam
			_snprintf(szConfigDir, sizeof(szConfigDir), "%s/config", steamPath);
			_findclose(findHandle);
		}
		else
		{
			// we're not running steam, so just put the config dir under the platform
			_snprintf(szConfigDir, sizeof(szConfigDir), "%sconfig", m_szPlatformDir);
		}
		*/

		// add the path
		g_pFullFileSystem->AddSearchPath(szConfigDir, "CONFIG");
		// make sure the config directory has been created
		_mkdir(szConfigDir);

		vgui2::ivgui()->DPrintf("Platform config directory: %s\n", szConfigDir);

		// user dialog configuration
		vgui2::system()->SetUserConfigFile("InGameDialogConfig.vdf", "CONFIG");

		// localization
//		g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/platform_%language%.txt");
//		g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/vgui_%language%.txt");


		//!! hack to work around problem with userinfo not being uploaded (and therefore *Tracker field)
		//!! this is done to make sure the *tracker userinfo field is set before we connect so that it
		//!! will get communicated to the server
		//!! this needs to be changed to a system where it is communicated to server when known but not before

		//!! addendum: this may very happen now with the platform changes; needs to be tested before this code
		//!! can be removed
		{
			// get the last known userID from the registry and set it in our userinfo string
//			HKEY key;
//			DWORD bufSize = sizeof(m_szPlatformDir);
//			unsigned int lastUserID = 0;
//			bufSize = sizeof(lastUserID);
//			if (ERROR_SUCCESS == g_pVCR->Hook_RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Valve\\Tracker", 0, KEY_READ, &key))
//			{
//				g_pVCR->Hook_RegQueryValueEx(key, "LastUserID", NULL, NULL, (unsigned char *)&lastUserID, &bufSize);
//
//				// close the registry key
//				g_pVCR->Hook_RegCloseKey(key);
//			}
//			if (lastUserID)
//			{
//				char buf[32];
//				sprintf(buf, "%d", lastUserID);
//				engine->PlayerInfo_SetValueForKey("*tracker", buf);
//			}
		}
	}

	// FOR SRC
	//	vgui::surface()->SetWorkspaceInsets( 0, 0, 0, g_pTaskbar->GetTall() );

	// Start loading tracker
	if (m_szPlatformDir[0] != 0)
	{
		vgui2::ivgui()->DPrintf2("Initializing platform...\n");

		// open a mutex
//		Sys_SetLastError((unsigned long) 0);

		// primary mutex is the platform.exe name
		char szExeName[sizeof(m_szPlatformDir) + 32];
		sprintf(szExeName, "%splatform.exe", m_szPlatformDir);
		// convert the backslashes in the path string to be forward slashes so it can be used as a mutex name
		for (char *ch = szExeName; *ch != 0; ch++)
		{
			*ch = tolower(*ch);
			if (*ch == '\\')
			{
				*ch = '/';
			}
		}

//		g_hMutex = Sys_CreateMutex("ValvePlatformUIMutex");
//		g_hWaitMutex = Sys_CreateMutex("ValvePlatformWaitMutex");
//		if (g_hMutex == NULL || g_hWaitMutex == NULL || Sys_GetLastError() == SYS_ERROR_INVALID_HANDLE)
//		{
//			// error, can't get handle to mutex
//			if (g_hMutex)
//			{
//				Sys_ReleaseMutex(g_hMutex);
//			}
//			if (g_hWaitMutex)
//			{
//				Sys_ReleaseMutex(g_hWaitMutex);
//			}
//			g_hMutex = NULL;
//			g_hWaitMutex = NULL;
//			Error("Tracker Error: Could not access Tracker, bad mutex\n");
//			return;
//		}
//		unsigned int waitResult = Sys_WaitForSingleObject(g_hMutex, 0);
//		if (!(waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED))
//		{
//			// mutex locked, need to close other tracker
//
//			// get the wait mutex, so that tracker.exe knows that we're trying to acquire ValveTrackerMutex
//			waitResult = Sys_WaitForSingleObject(g_hWaitMutex, 0);
//			if (waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED)
//			{
//				Sys_EnumWindows(SendShutdownMsgFunc, 1);
//			}
//		}
//		m_bTryingToLoadTracker = true;
		// now we are set up to check every frame to see if we can Start tracker
	}

	staticPanel->SetBackgroundRenderState(CBasePanel::BACKGROUND_DESKTOPIMAGE);

	// start mp3 playing
	//engine->pfnClientCmd("mp3 loop media/gamestartup.mp3\n");

	// SRC version
	//engine->ClientCmd("loop media/gamestartup.mp3\n");

}

bool CGameUI::FindPlatformDirectory(char *platformDir, int bufferSize)
{
	platformDir[0] = '\0';

	if ( platformDir[0] == '\0' )
	{
		// we're not under steam, so setup using path relative to game
		if ( 1 )
		{
#ifdef WIN32
			if ( ::GetModuleFileName( ( HINSTANCE )GetModuleHandle( NULL ), platformDir, bufferSize ) )
			{
				char *lastslash = strrchr(platformDir, '\\'); // this should be just before the filename
				if ( lastslash )
				{
					*lastslash = 0;
					Q_strncat(platformDir, "\\platform\\", bufferSize, COPY_ALL_CHARACTERS );
					return true;
				}
			}
#else
			if ( getcwd( platformDir, bufferSize ) )
			{
				V_AppendSlash( platformDir, bufferSize );
				Q_strncat(platformDir, "platform", bufferSize, COPY_ALL_CHARACTERS );
				V_AppendSlash( platformDir, bufferSize );
				return true;
			}
#endif
		}

		Error( "Unable to determine platform directory\n" );
		return false;
	}

	return (platformDir[0] != 0);
}

void CGameUI::Shutdown()
{
	// notify all the modules of Shutdown
	g_VModuleLoader.ShutdownPlatformModules();

	// unload the modules them from memory
	g_VModuleLoader.UnloadPlatformModules();

	// free mod info
//	ModInfo().FreeModInfo();

}
bool CGameUI::IsGameUIActive()
{
	if ( m_bActivatedUI )
	{
		return staticPanel->IsVisible();
	}
	else
		return false;
}


int CGameUI::ActivateGameUI()
{
	if (IsGameUIActive())
		return 1;

	m_bActivatedUI = true;

	//	TRACE_FUNCTION("CGameUI::ActivateGameUI");

	// hide/show the main panel to Activate all game ui
	staticPanel->SetVisible(true);
	// pause the game
	engine->pfnClientCmd("setpause");


	// return that things have been handled
	return 1;
}

void CGameUI::HideGameUI()
{
	//	TRACE_FUNCTION("CGameUI::HideGameUI");
	// we can't hide the UI if we're not in a level
	const char *levelName = engine->pfnGetLevelName();
	if (levelName && levelName[0])
	{
		//show both the background panel and the taskbar
		staticPanel->SetVisible(false);

		// unpause the game
		engine->pfnClientCmd("unpause");
	}
}
int CGameUI::HasExclusiveInput()
{
	return IsGameUIActive();
}

void CGameUI::RunFrame()
{
	// resize the background panel to the screen size
	int wide, tall;
	vgui2::surface()->GetScreenSize(wide, tall);
	staticPanel->SetSize(wide,tall);

	// Run frames
	g_VModuleLoader.RunFrame();

	/*	if( m_pMaster )
	{
		m_pMaster->Frame();
	}
	*/

	if( vgui2::surface()->GetModalPanel() )
	{
		vgui2::surface()->PaintTraverse( staticPanel->GetVPanel());
	}
}
void CGameUI::ConnectToServer(const char *game, int IP, int port)
{

	//engine->pfnClientCmd("mp3 stop\n");
	// SRC version
	//	engine->ClientCmd("stop\n");
	baseuifuncs->HideGameUI();

	// start running our version query if we are not running steam
	/*	if( !engine->CheckParm("-steam", NULL) )
	{
		MasterVersionCheckQuery();
	}
*/
	m_iGameIP = IP;
	m_iGamePort = port;

	g_VModuleLoader.PostMessageToAllModules(new KeyValues("ConnectedToGame", "ip", IP, "port", port));
}

//-----------------------------------------------------------------------------
// Purpose: Called when the game disconnects from a server
//-----------------------------------------------------------------------------
void CGameUI::DisconnectFromServer()
{
	ActivateGameUI();

	m_iGameIP = 0;
	m_iGamePort = 0;
	g_VModuleLoader.PostMessageToAllModules(new KeyValues("DisconnectedFromGame"));
}

void CGameUI::LoadingStarted(const char *resourceType, const char *resourceName)
{
	g_VModuleLoader.PostMessageToAllModules(new KeyValues("LoadingStarted", "type", resourceType, "name", resourceName));

	if (!stricmp(resourceType, "transition"))
	{
		// activate the loading image
		staticPanel->SetBackgroundRenderState(CBasePanel::BACKGROUND_LOADINGTRANSITION);
	}
	else
	{
		staticPanel->SetBackgroundRenderState(CBasePanel::BACKGROUND_LOADING);
	}
}

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	// notify all the modules
	g_VModuleLoader.PostMessageToAllModules(new KeyValues("LoadingFinished", "type", resourceType, "name", resourceName));

	// stop drawing loading screen
	staticPanel->SetBackgroundRenderState(CBasePanel::BACKGROUND_DESKTOPIMAGE);

	// hide the UI
	baseuifuncs->HideGameUI();
}

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	//	TRACE_FUNCTION("CGameUI::StartProgressBar");

	if (!g_hLoadingDialog.Get())
	{
//		g_hLoadingDialog = new CLoadingDialog(staticPanel);
	}

	// close the start menu
//	staticPanel->SetBackgroundRenderState(CBasePanel::BACKGROUND_LOADING);
	m_pszCurrentProgressType = progressType;
	if (m_flProgressStartTime < 0.001f)
	{
		m_flProgressStartTime = (float)vgui2::system()->GetCurrentTime();
	}

	// open a loading dialog
//	g_hLoadingDialog->SetProgressRange(0 , progressSteps);
//	g_hLoadingDialog->SetProgressPoint(0);
//	g_hLoadingDialog->DisplayProgressBar(progressType, "invalid");
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	if (!g_hLoadingDialog.Get())
		return 0;

//	g_hLoadingDialog->SetProgressPoint(progressPoint);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: stops progress bar, displays error if necessary
//-----------------------------------------------------------------------------
void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	if (!g_hLoadingDialog.Get() && bError)
	{
//		g_hLoadingDialog = new CLoadingDialog(staticPanel);
	}

	if (!g_hLoadingDialog.Get())
		return;

	if (bError)
	{
		// turn the dialog to error display mode
//		g_hLoadingDialog->DisplayError(failureReason,extendedReason);
	}
	else
	{
		// close loading dialog
//		g_hLoadingDialog->Close();
//		g_hLoadingDialog = NULL;
	}

	// stop drawing loading screen
	staticPanel->SetBackgroundRenderState(CBasePanel::BACKGROUND_DESKTOPIMAGE);
}

//-----------------------------------------------------------------------------
// Purpose: sets loading info text
//-----------------------------------------------------------------------------
int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	if (!g_hLoadingDialog.Get())
		return 0;

//	g_hLoadingDialog->SetStatusText(statusText);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameUI::SetSecondaryProgressBar(float progress /* range [0..1] */)
{
	if (!g_hLoadingDialog.Get())
		return;

//	g_hLoadingDialog->SetSecondaryProgress(progress);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	if (!g_hLoadingDialog.Get())
		return;

//	g_hLoadingDialog->SetSecondaryProgressText(statusText);
}
int CGameUI::ActivateDemoUI() {
	return 1;
}
void CGameUI::OnDisconnectFromServer(int eSteamLoginFailure, const char *username) {

}
void CGameUI::SetIsRunningOnSteamDeck(bool bIsRunningOnDeck) {

}
bool CGameUI::IsRunningOnSteamDeck() const {return false;
}
void CGameUI::ValidateCDKey(bool force, bool inConnect) {

}