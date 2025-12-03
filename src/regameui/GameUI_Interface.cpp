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
#include "ModInfo.h"
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
cl_enginefunc_t gEngfuncs;
#include "tier0/dbg.h"
#include "cl_dll/IGameClientExports.h"
#include "IGameUI.h"
#include "IGameUIFuncs.h"
#include "icvar.h"
#include "VGuiSystemModuleLoader.h"
#include "Sys_Utils.h"
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

typedef CBasePanel UI_BASEMOD_PANEL_CLASS;
inline UI_BASEMOD_PANEL_CLASS &GetUiBaseModPanelClass() { return *BasePanel(); }
inline UI_BASEMOD_PANEL_CLASS &ConstructUiBaseModPanelClass()
{
	if (!BasePanel())
		new CBasePanel();
	return *BasePanel();
}
vgui2::VPANEL GetGameUIBasePanel()
{
	return GetUiBaseModPanelClass().GetVPanel();
}
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

static CUtlVector<CreateInterfaceFn> s_FactoryList;

static void *MegaFactory(const char *pName, int *pReturnCode)
{
	for (int i = 0; i < s_FactoryList.Size(); i++)
	{
		void *pIface = s_FactoryList[i](pName, pReturnCode);
		if (pIface)
			return pIface;
	}

	return nullptr;
}

static CreateInterfaceFn s_pFactory = MegaFactory;
void CGameUI::Initialize(CreateInterfaceFn *factories, int count)
{
	s_FactoryList.SetSize(count);
	memcpy(s_FactoryList.Base(), factories, count * sizeof(CreateInterfaceFn));
}

//-----------------------------------------------------------------------------
// Purpose: Initialization and setup
//-----------------------------------------------------------------------------
void CGameUI::Start(cl_enginefunc_t *pEnginefuncs, int iVersion, void *system)
{
	if (iVersion != CLDLL_INTERFACE_VERSION)
	{
		Error("CGameUI::Start: Incorrect engine version (expected %d, got %d)\n",
		    CLDLL_INTERFACE_VERSION, iVersion);
	}

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(cl_enginefunc_t));

	InternalInitialize();
	InternalStart();
}

//-----------------------------------------------------------------------------
// Purpose: Initialization
//-----------------------------------------------------------------------------
void CGameUI::InternalInitialize()
{
	ConnectTier1Libraries(&s_pFactory, 1);
	ConnectTier2Libraries(&s_pFactory, 1);
//	CvarSystem::RegisterCvars();

	// TODO: Remove completely, only leave gEngfuncs
	gEngfuncs.pfnClientCmd("map c1a0"); // TODO: randomize - ScriptedSnark

//	SteamAPI_InitSafe();
//	steamapicontext->Init();

//	ma_engine_init(NULL, &miniaudio);

	vgui2::VGui_InitInterfacesList("GameUI", &s_pFactory, 1);

	// load localization file
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/gameui_%language%.txt");

	// load mod info
	ModInfo().LoadCurrentGameInfo();

	// load localization file for kb_act.lst
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/valve_%language%.txt");

	bool bFailed = false;
	enginevguifuncs = (IEngineVGui *)s_pFactory(VENGINE_VGUI_VERSION, NULL);
	enginesurfacefuncs = g_pVGuiSurface;
	gameuifuncs = (IGameUIFuncs *)s_pFactory(IGAMEUIFUNCS_NAME, NULL);
//	xboxsystem = (IXboxSystem *)s_pFactory(XBOXSYSTEM_INTERFACE_VERSION, NULL);
	bFailed = !enginesurfacefuncs || !gameuifuncs || !enginevguifuncs;
	if (bFailed)
	{
		Error("CGameUI::Start() failed to get necessary interfaces\n");
	}

	// setup base panel
//	CBasePanel *factoryBasePanel = new CBasePanel(); // explicit singleton instantiation
	CBasePanel *factoryBasePanel = new CBasePanel();
	factoryBasePanel->SetBounds(0, 0, 640, 480);
	factoryBasePanel->SetPaintBorderEnabled(false);
	factoryBasePanel->SetPaintBackgroundEnabled(true);
	factoryBasePanel->SetPaintEnabled(true);
	factoryBasePanel->SetVisible(true);

	factoryBasePanel->SetMouseInputEnabled(IsPC());
	// factoryBasePanel.SetKeyBoardInputEnabled( IsPC() );
	factoryBasePanel->SetKeyBoardInputEnabled(true);

	vgui2::VPANEL rootpanel = enginevguifuncs->GetPanel(PANEL_GAMEUIDLL);
	factoryBasePanel->SetParent(rootpanel);

	// Client DLL interface
	g_pGameClientExports = (IGameClientExports *)s_pFactory(GAMECLIENTEXPORTS_INTERFACE_VERSION, NULL);
}
int __stdcall SendShutdownMsgFunc(WHANDLE hwnd, int lparam)
{
	Sys_PostMessage(hwnd, Sys_RegisterWindowMessage("ShutdownValvePlatform"), 0, 1);
	return 1;
}

void CGameUI::InternalStart()
{
	// determine Steam location for configuration

//	if (!FindPlatformDirectory(m_szPlatformDir, sizeof(m_szPlatformDir)))
//		return;

	if (IsPC())
	{
		// setup config file directory
		char szConfigDir[512];
		Q_strncpy(szConfigDir, m_szPlatformDir, sizeof(szConfigDir));
		Q_strncat(szConfigDir, "config", sizeof(szConfigDir), COPY_ALL_CHARACTERS);

		Msg("Steam config directory: %s\n", szConfigDir);

		g_pFullFileSystem->AddSearchPath(szConfigDir, "CONFIG");
		g_pFullFileSystem->CreateDirHierarchy("", "CONFIG");

		// user dialog configuration
		vgui2::system()->SetUserConfigFile("InGameDialogConfig.vdf", "CONFIG");

		g_pFullFileSystem->AddSearchPath("platform", "PLATFORM");
	}

	// localization
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/platform_%language%.txt");
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/vgui_%language%.txt");

	Sys_SetLastError(0L);
	if (IsPC())
	{
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
//			Error("Steam Error: Could not access Steam, bad mutex\n");
//			return;
//		}
//		unsigned int waitResult = Sys_WaitForSingleObject(g_hMutex, 0);
//		if (!(waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED))
//		{
//			// mutex locked, need to deactivate Steam (so we have the Friends/ServerBrowser data files)
//			// get the wait mutex, so that Steam.exe knows that we're trying to acquire ValveTrackerMutex
//			waitResult = Sys_WaitForSingleObject(g_hWaitMutex, 0);
//			if (waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED)
//			{
//				Sys_EnumWindows(SendShutdownMsgFunc, 1);
//			}
//		}

		// Play the start-up music
//		PlayGameStartupSound();

		// now we are set up to check every frame to see if we can friends/server browser
//		m_bTryingToLoadFriends = true;
//		m_iFriendsLoadPauseFrames = 1;
	}
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
	ModInfo().FreeModInfo();

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
	gEngfuncs.pfnClientCmd("setpause");


	// return that things have been handled
	return 1;
}

void CGameUI::HideGameUI()
{
	//	TRACE_FUNCTION("CGameUI::HideGameUI");
	// we can't hide the UI if we're not in a level
	const char *levelName = gEngfuncs.pfnGetLevelName();
	if (levelName && levelName[0])
	{
		//show both the background panel and the taskbar
		staticPanel->SetVisible(false);

		// unpause the game
		gEngfuncs.pfnClientCmd("unpause");
	}
}
int CGameUI::HasExclusiveInput()
{
	return IsGameUIActive();
}

void CGameUI::RunFrame(void)
{
	int wide, tall;
#if defined(TOOLFRAMEWORK_VGUI_REFACTOR)
	// resize the background panel to the screen size
	vgui::VPANEL clientDllPanel = enginevguifuncs->GetPanel(PANEL_ROOT);

	int x, y;
	vgui::ipanel()->GetPos(clientDllPanel, x, y);
	vgui::ipanel()->GetSize(clientDllPanel, wide, tall);
	staticPanel->SetBounds(x, y, wide, tall);
#else
	vgui2::surface()->GetScreenSize(wide, tall);

	GetUiBaseModPanelClass().SetSize(wide, tall);
#endif

	// Run frames
	g_VModuleLoader.RunFrame();
	GetUiBaseModPanelClass().RunFrame();
//	GetUiBaseModPanelClass().RunFrame();

//	GameConsole().RunFrame();

//	if (IsPC() && m_bTryingToLoadFriends && m_iFriendsLoadPauseFrames-- < 1 && g_hMutex && g_hWaitMutex)
//	{
		// try and load Steam platform files
//		unsigned int waitResult = Sys_WaitForSingleObject(g_hMutex, 0);
//		if (waitResult == SYS_WAIT_OBJECT_0 || waitResult == SYS_WAIT_ABANDONED)
//		{
//			// we got the mutex, so load Friends/Serverbrowser
//			// clear the loading flag
//			m_bTryingToLoadFriends = false;
//			g_VModuleLoader.LoadPlatformModules(&s_pFactory, 1, false);
//
//			// release the wait mutex
//			Sys_ReleaseMutex(g_hWaitMutex);
//
//			// notify the game of our game name
//			const char *fullGamePath = engine->GetGameDirectory();
//			const char *pathSep = strrchr(fullGamePath, '/');
//			if (!pathSep)
//			{
//				pathSep = strrchr(fullGamePath, '\\');
//			}
//			if (pathSep)
//			{
//				KeyValues *pKV = new KeyValues("ActiveGameName");
//				pKV->SetString("name", pathSep + 1);
//				pKV->SetInt("appid", engine->GetAppID());
//				KeyValues *modinfo = new KeyValues("ModInfo");
//				if (modinfo->LoadFromFile(g_pFullFileSystem, "gameinfo.txt"))
//				{
//					pKV->SetString("game", modinfo->GetString("game", ""));
//				}
//				modinfo->deleteThis();
//
//				g_VModuleLoader.PostMessageToAllModules(pKV);
//			}
//
//			// notify the ui of a game connect if we're already in a game
//			if (m_iGameIP)
//			{
//				SendConnectedToGameMessage();
//			}
//		}
//	}
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
