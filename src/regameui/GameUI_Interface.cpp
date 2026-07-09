//
// Created by den4ik on 01.12.25.
//
#include "GameConsole.h"
#include "IRunGameEngine.h"
#include "LoadingDialog.h"
#include "sdl_rt.h"
#ifdef WIN32
#include <winsani_in.h>

#if !defined( _X360 )
#include <windows.h>
#endif
#include <io.h>
#include <direct.h>
#include <winsani_out.h>
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
// #include <winsani_out.h>
IGameUIFuncs *gameuifuncs = NULL;
IEngineVGui *enginevguifuncs = NULL;
vgui2::ISurface *enginesurfacefuncs = NULL;
IBaseUI *baseuifuncs = NULL;
IRunGameEngine *rungameengine = NULL;
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
// cl_enginefunc_t *engine = &gEngfuncs;
ICvar *cvar = NULL;

class CGameUI;
extern IGameUI *g_pGameUI;


class CLoadingDialog;
vgui2::DHANDLE<CLoadingDialog> g_hLoadingDialog;

static CGameUI g_GameUI;

static int g_hMutex = NULL;
static int g_hWaitMutex = NULL;
IServerBrowser *g_pServerBrowser;
typedef CBasePanel UI_BASEMOD_PANEL_CLASS;
inline UI_BASEMOD_PANEL_CLASS &GetUiBaseModPanelClass() { return *BasePanel(); }
inline UI_BASEMOD_PANEL_CLASS &ConstructUiBaseModPanelClass()
{
	if (!BasePanel())
		new CBasePanel();
	return *BasePanel();
}
namespace vgui2
{

HScheme VGui_GetDefaultScheme()
{
	return 0;
}

}
CBasePanel *factoryBasePanel = NULL;
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
    // gEngfuncs.pfnClientCmd("map c1a0"); // TODO: randomize - ScriptedSnark

//	SteamAPI_InitSafe();
//	steamapicontext->Init();

//	ma_engine_init(NULL, &miniaudio);
	vgui2::VGui_InitInterfacesList("GameUI", &s_pFactory, 1);

	// load localization file
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/gameui_%language%.txt");
	// load mod info
	ModInfo()
	    .LoadCurrentGameInfo();

	// load localization file for kb_act.lst
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/valve_%language%.txt");
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "resource/regameui_%language%.txt");

	bool bFailed = false;
	enginevguifuncs = (IEngineVGui *)s_pFactory(VENGINE_VGUI_VERSION, NULL);
	enginesurfacefuncs = g_pVGuiSurface;
	gameuifuncs = (IGameUIFuncs *)s_pFactory(IGAMEUIFUNCS_NAME, NULL);
	baseuifuncs = (IBaseUI *)s_pFactory(IBASEUI_NAME, NULL);
	rungameengine = (IRunGameEngine *)s_pFactory(RUNGAMEENGINE_INTERFACE_VERSION, NULL);
	//	xboxsystem = (IXboxSystem *)s_pFactory(XBOXSYSTEM_INTERFACE_VERSION, NULL);
	bFailed = !enginesurfacefuncs || !gameuifuncs || !enginevguifuncs;
	if (bFailed)
	{
		Error("CGameUI::Start() failed to get necessary interfaces\n");
	}

	// setup base panel
//	CBasePanel *factoryBasePanel = new CBasePanel(); // explicit singleton instantiation
	factoryBasePanel = new CBasePanel();
    factoryBasePanel->SetBounds(0, 0, 400, 300);
	factoryBasePanel->SetPaintBorderEnabled(false);
	factoryBasePanel->SetPaintBackgroundEnabled(true);
	factoryBasePanel->SetPaintEnabled(true);
	factoryBasePanel->SetVisible(true);
    factoryBasePanel->SetScheme(vgui2::scheme()->LoadSchemeFromFile("ui/resource/ClientSourceScheme.res", "ClientSourceScheme"));
    factoryBasePanel->SetMouseInputEnabled(true);
    factoryBasePanel->SetKeyBoardInputEnabled(true);
	vgui2::VPANEL rootpanel = enginevguifuncs->GetPanel(PANEL_GAMEUIDLL);
    // rootpanel->

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

    if (!FindPlatformDirectory(m_szPlatformDir, sizeof(m_szPlatformDir)))
        return;
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
	g_pVGuiLocalize->AddFile(g_pFullFileSystem, "Resource/regameui_%language%.txt");
	g_VModuleLoader.LoadPlatformModules(&s_pFactory, 1, false);
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
if (g_pServerBrowser)
{
	// g_pServerBrowser->ActiveGameName(ModInfo().GetGameDescription(), engine->pfnGetGameDirectory());
	// g_pServerBrowser->Reactivate();
}
m_bTryingToLoadTracker = true;
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
			//printf(platformDir);
#endif
		}

		Error( "Unable to determine platform directory\n" );
		return false;
	}
    // check for ServerBrowser DLL on local area
    if (platformDir[0] == '\0')
    {
#ifdef WIN32
        char *pszServerBrowserDLL = "..\\platform\\servers\\serverbrowser.dll";
#else
                char *pszServerBrowserDLL = "..\\platform\\servers\\serverbrowser.so";
#endif
        // Require that we find the ServerBrowser DLL.
        if (g_pFullFileSystem->FileExists(pszServerBrowserDLL))
        {
            char szPlatformPath[MAX_PATH], szFinalPath[MAX_PATH];
            g_pFullFileSystem->GetLocalPath(pszServerBrowserDLL, szPlatformPath, sizeof(szPlatformPath));
            szPlatformPath[MAX_PATH - 1] = 0;

            // remove any \..\ from the path
            szFinalPath[0] = 0;
            int finalPathPos = 0;
            for (int i = 0; szPlatformPath[i] != 0; i++)
            {
                if (!strncmp(szPlatformPath + i, "\\..\\", 4))
                {
                    // skip over the "\\.."
                    i += 3;

                    // walk the final dir back until the previous '\\'
                    while (szFinalPath[finalPathPos] != '\\' && finalPathPos)
                    {
                        finalPathPos--;
                    }
                }

                szFinalPath[finalPathPos++] = szPlatformPath[i];
            }
#ifdef WIN32
            char *binpos = strstr(szFinalPath, "servers\\serverbrowser.dll");
#else
            char *binpos = strstr(szFinalPath, "servers\\serverbrowser.so");
#endif
            if (binpos)
            {
                *binpos = 0;
                strcpy(platformDir, szFinalPath);
            }
        }
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
		return GetUiBaseModPanelClass().IsVisible();
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
	GetUiBaseModPanelClass().SetVisible(true);
	// tell our panel that we're activated;
	GetUiBaseModPanelClass().OnGameUIActivated();
	// pause the game
	gEngfuncs.pfnClientCmd("setpause");
    GetUiBaseModPanelClass().SetMouseInputEnabled(true);
    GetUiBaseModPanelClass().MoveToFront();
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
		GetUiBaseModPanelClass().SetVisible(false);
        GetUiBaseModPanelClass().SetMouseInputEnabled(false);

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
	GetUiBaseModPanelClass().SetBounds(x, y, wide, tall);
#else
	vgui2::surface()->GetScreenSize(wide, tall);

	GetUiBaseModPanelClass().SetSize(wide, tall);
#endif

	// Run frames
	g_VModuleLoader.RunFrame();
	GetUiBaseModPanelClass().RunFrame();
//	GetUiBaseModPanelClass().RunFrame();

//	GameConsole().RunFrame();

	m_bTryingToLoadTracker = false;
}
void CGameUI::ConnectToServer(const char *game, int IP, int port)
{

	//engine->pfnClientCmd("mp3 stop\n");
	// SRC version
	//	engine->ClientCmd("stop\n");
//	baseuifuncs->HideGameUI();

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
		GetUiBaseModPanelClass().SetBackgroundRenderState(CBasePanel::BACKGROUND_BLACK);
	}
	else
	{
		GetUiBaseModPanelClass().SetBackgroundRenderState(CBasePanel::BACKGROUND_BLACK);
	}
}

void CGameUI::LoadingFinished(const char *resourceType, const char *resourceName)
{
	// notify all the modules
	g_VModuleLoader.PostMessageToAllModules(new KeyValues("LoadingFinished", "type", resourceType, "name", resourceName));

	// stop drawing loading screen
	GetUiBaseModPanelClass().SetBackgroundRenderState(CBasePanel::BACKGROUND_DESKTOPIMAGE);

	// hide the UI
//	baseuifuncs->HideGameUI();
}

void CGameUI::StartProgressBar(const char *progressType, int progressSteps)
{
	//	TRACE_FUNCTION("CGameUI::StartProgressBar");

	if (!g_hLoadingDialog.Get())
	{
		g_hLoadingDialog = new CLoadingDialog(factoryBasePanel);
	}

	// close the start menu
	GetUiBaseModPanelClass().SetBackgroundRenderState(CBasePanel::BACKGROUND_LOADING);
	m_pszCurrentProgressType = progressType;
	if (m_flProgressStartTime < 0.001f)
	{
		m_flProgressStartTime = (float)vgui2::system()->GetCurrentTime();
	}

	// open a loading dialog
	g_hLoadingDialog->SetProgressRange(0, progressSteps);
	g_hLoadingDialog->SetProgressPoint(0);
	g_hLoadingDialog->DisplayProgressBar(progressType, "invalid");
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CGameUI::ContinueProgressBar(int progressPoint, float progressFraction)
{
	if (!g_hLoadingDialog.Get())
		return 0;

	g_hLoadingDialog->SetProgressPoint(progressPoint);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: stops progress bar, displays error if necessary
//-----------------------------------------------------------------------------
void CGameUI::StopProgressBar(bool bError, const char *failureReason, const char *extendedReason)
{
	if (!g_hLoadingDialog.Get() && bError)
	{
		g_hLoadingDialog = new CLoadingDialog(factoryBasePanel);
	}

	if (!g_hLoadingDialog.Get())
		return;

	if (bError)
	{
		// turn the dialog to error display mode
		g_hLoadingDialog->DisplayError(failureReason, extendedReason);
	}
	else
	{
		// close loading dialog
		g_hLoadingDialog->Close();
		g_hLoadingDialog = NULL;
	}

	// stop drawing loading screen
	GetUiBaseModPanelClass().SetBackgroundRenderState(CBasePanel::BACKGROUND_DESKTOPIMAGE);
}

//-----------------------------------------------------------------------------
// Purpose: sets loading info text
//-----------------------------------------------------------------------------
int CGameUI::SetProgressBarStatusText(const char *statusText)
{
	if (!g_hLoadingDialog.Get())
		return 0;

	g_hLoadingDialog->SetStatusText(statusText);
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameUI::SetSecondaryProgressBar(float progress /* range [0..1] */)
{
	if (!g_hLoadingDialog.Get())
		return;

	g_hLoadingDialog->SetSecondaryProgress(progress);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGameUI::SetSecondaryProgressBarText(const char *statusText)
{
	if (!g_hLoadingDialog.Get())
		return;

	g_hLoadingDialog->SetSecondaryProgressText(statusText);
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
bool CGameUI::IsInLevel() {
	const char *levelName = gEngfuncs.pfnGetLevelName();
	return levelName && levelName[0];
}
