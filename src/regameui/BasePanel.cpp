//
// Created by den4ik on 01.12.25.
//
#include <stdio.h>

#include "BasePanel.h"
#include "EngineInterface.h"
#include "FileSystem.h"
#include "GameConsole.h"
#include "IEngineVGui.h"
#include "GameUI_Interface.h"
#include "GameMenu.h"
#include "ModInfo.h"
#include "VGuiSystemModuleLoader.h"
#include "options/optionsdialog.h"
#include "createmultiplayer/CreateMultiplayerGameDialog.h"
#include "NewGameDialog.h"

#include "convar.h"
#include "BackgroundMenuButon.h"
#include "sdl_rt.h"
#include "serverconnectpanel.h"
#include "vgui_controls/MenuBar.h"
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
extern cl_enginefunc_t gEngfuncs;
using namespace vgui2;
static CBasePanel	*g_pBasePanel = NULL;
// extern CGameConsole g_GameConsole;
#define MENU_HEIGHT 28

CBasePanel *BasePanel()
{
	return g_pBasePanel;
}
static CBackgroundMenuButton *CreateMenuButton(CBasePanel *parent, const char *panelName, const wchar_t *panelText)
{
	CBackgroundMenuButton *pButton = new CBackgroundMenuButton( parent, panelName );
	pButton->SetCommand("OpenGameMenu");
	pButton->SetText(panelText);

	return pButton;
}
extern IServerBrowser *g_pServerBrowser;

//-----------------------------------------------------------------------------
CBasePanel::CBasePanel() : EditablePanel(NULL, "BaseGameUIPanel")
{
	SetProportional(true);
	// SetParent(PANEL_GAMEUIDLL);
	g_pBasePanel = this;
    m_eBackgroundState = BACKGROUND_BLACK;
    RequestFocus();
    int w;
    int h;
    vgui2::surface()->GetScreenSize(w, h);

    SetZPos(1000);
    SetBuildModeEditable(1);
    SetBuildModeDeletable(1);

    SetProportional(true);
    LoadControlSettings("resource/mainmenu.res");
    m_pMenuBar = new vgui2::MenuBar(this, "MainMenuBar");
    m_pMenuBar->SetParent( this );
    m_pMenuBar->SetSize( w, MENU_HEIGHT );
    m_pMenuBar->SetMouseInputEnabled(true);
	vgui2::Menu *pGameMenu = new vgui2::Menu(m_pMenuBar, "GameMenu");
	m_pMenuBar->AddMenu("Game", pGameMenu);
	pGameMenu->AddMenuItem("Start a new game", "NewGameDialog", this);
	pGameMenu->AddMenuItem("Save game", "SaveGameDialog", this);
	pGameMenu->AddMenuItem("Load game", "LoadGameDialog", this);

	vgui2::Menu *pMultiplayerMenu = new vgui2::Menu(m_pMenuBar, "MuliplayerMenu");
	m_pMenuBar->AddMenu("Multiplayer", pMultiplayerMenu);
	pMultiplayerMenu->AddMenuItem("Create a server", "CreateServerDialog", this);
	pMultiplayerMenu->AddMenuItem("Connect to a server", "ConnectDialog", this);
	vgui2::Menu *pOptionsMenu = new vgui2::Menu(m_pMenuBar, "OptionsMenu");
	m_pMenuBar->AddMenu("Options", pOptionsMenu);
	pOptionsMenu->AddMenuItem("Multiplayer options", ("OpenOptionsDialog"), this, new KeyValues("Multiplayer"));
	pOptionsMenu->AddMenuItem("Video settings", "OpenOptionsDialog", this, new KeyValues("Video"));
	pOptionsMenu->AddMenuItem("Audio settings", "OpenOptionsDialog", this, new KeyValues("Audio"));
	// m_hOptionsDialog = new COptionsDialog(this);
	// m_hOptionsDialog->Activate();
	// OnOpenOptionsDialog();
	vgui2::Menu *pAdvOptionsMenu = new vgui2::Menu(m_pMenuBar, "AdvOptionsMenu");
	m_pMenuBar->AddMenu("Advanced Options", pAdvOptionsMenu);
	pAdvOptionsMenu->AddMenuItem("Addon settings", "AdvOptionsDialog", this);
	SetBackgroundRenderState(BACKGROUND_DESKTOPIMAGE);
	// DLLHACKHACKHACK: So, if we dont have active frames at this point,
	// we are loosing mouse input on WHOOOLE BasePanel, so just setup that
	// "invisible" frame to force our dear VGUI2 think that we have some frame
	SetupThatFrickinPanel();
}
void CBasePanel::SetupThatFrickinPanel()
{
	vgui2::Frame *frame = new vgui2::Frame(this, "");
	frame->Activate();
	frame->SetAlpha(0);
	frame->SetVisible(1);
	int wide, tall;
	vgui2::surface()->GetScreenSize(wide, tall);
	frame->SetPos(-100, -100);
	frame->SetTitleBarVisible(0);
	frame->SetMinimumSize(1, 1);
	frame->SetSizeable(0);
	frame->SetSize(1, 1);
	frame->SetFgColor(Color(0, 0, 0, 0));
	frame->SetBgColor(Color(0, 0, 0, 0));
	frame->SetPaintEnabled(false);
	frame->SetEnabled(0);
	frame->SetPaintBackgroundEnabled(0);
	frame->SetKeyBoardInputEnabled(0);
	frame->SetBorder(NULL);
	frame->SetCloseButtonVisible(0);
}
void CBasePanel::OnOpenOptionsDialog(int tab)
{
	if (!m_hOptionsDialog)
	{
		m_hOptionsDialog = new COptionsDialog(this);
	}
	m_hOptionsDialog.Get()->Activate(tab);
	PositionDialog(m_hOptionsDialog);
}

void CBasePanel::OnChildAdded(VPANEL child)
{
    BaseClass::OnChildAdded(child);
}

void CBasePanel::PaintBackground()
{
	const char *levelName = gEngfuncs.pfnGetLevelName();
	if (levelName && levelName[0])
	{
//		 render filled background in game
		int swide, stall;
		surface()->GetScreenSize(swide, stall);
		surface()->DrawSetColor(0, 0, 0, 128);
		surface()->DrawFilledRect(0, 0, swide, stall);
		return;
	}

	switch (m_eBackgroundState)
	{
	case BACKGROUND_BLACK:
	{
        // if the loading dialog is visible, draw the background black
        int swide, stall;
        surface()->GetScreenSize(swide, stall);
		surface()->DrawSetColor(0, 0, 0, 128);
		surface()->DrawFilledRect(0, 0, swide, stall);
	}
	break;

	case BACKGROUND_LOADING:
		DrawBackgroundImage();
		break;

	case BACKGROUND_DESKTOPIMAGE:
		DrawBackgroundImage();
		break;

	case BACKGROUND_LOADINGTRANSITION:
	{
	}
	break;

	case BACKGROUND_NONE:
	default:
		break;
	}
}

void CBasePanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	// turn on hardware filtering if we're scaling the images
	int wide, tall;
	surface()->GetScreenSize(wide, tall);
	bool hardwareFilter = false; //(wide != 800);

//	bimage_t &bimage = m_ImageID[0][0];
//	bimage.imageID = surface()->CreateNewTextureID();

//	char filename[512];
//	sprintf(filename, "console/console_background" );
//	surface()->DrawSetTextureFile(bimage.imageID, filename, hardwareFilter, false);
//	surface()->DrawGetTextureSize(bimage.imageID, bimage.width, bimage.height);

	// for (int y = 0; y < BACKGROUND_ROWS; y++)
	// {
	// 	for (int x = 0; x < BACKGROUND_COLUMNS; x++)
	// 	{
	// 		bimage_t &bimage = m_ImageID[y][x];
	// 		bimage.imageID = surface()->CreateNewTextureID();

	// 		char filename[512];
	// 		sprintf(filename, "resource/background/800_%d_%c_loading", y + 1, 'a' + x);
	// 		surface()->DrawSetTextureFile(bimage.imageID, filename, hardwareFilter, false);
	// 		surface()->DrawGetTextureSize(bimage.imageID, bimage.width, bimage.height);
	// 	}
	// }

	SetBgColor(Color(0, 0, 0, 0));

	m_flFrameFadeInTime = 0.3f;
	m_BackdropColor = Color(0, 0, 0, 128);

	int screenWide, screenTall;
	vgui2::surface()->GetScreenSize(screenWide, screenTall);

	float aspectRatio = (float)screenWide / (float)screenTall;
	bool bIsWidescreen = aspectRatio >= 1.5999f;
	FileHandle_t file = g_pFullFileSystem->Open("resource/HD_BackgroundLayout.txt", "rt");
	if (!file)
		return;

	int fileSize = g_pFullFileSystem->Size(file);
	char *buffer = (char *)alloca(fileSize + 1);
	g_pFullFileSystem->Read(buffer, fileSize, file);
	g_pFullFileSystem->Close(file);
	buffer[fileSize] = 0;

	char token[512];
	while (buffer && *buffer)
	{
		buffer = g_pFullFileSystem->ParseFile(buffer, token, NULL);
		if (!buffer || !buffer[0])
			break;

		if (!stricmp(token, "resolution"))
		{
			buffer = g_pFullFileSystem->ParseFile(buffer, token, NULL);
			m_iBaseResX = atoi(token);
			buffer = g_pFullFileSystem->ParseFile(buffer, token, NULL);
			m_iBaseResY = atoi(token);
		}
		else
		{
			bimage_t &bimage = m_ImageID[m_ImageID.AddToTail()];
			bimage.imageID = vgui2::surface()->CreateNewTextureID();

			char *ext = strstr(token, ".tga");
			if (ext)
				*ext = 0;

			vgui2::surface()->DrawSetTextureFile(bimage.imageID, token, 1, false);
			vgui2::surface()->DrawGetTextureSize(bimage.imageID, bimage.width, bimage.height);

			buffer = g_pFullFileSystem->ParseFile(buffer, token, NULL);
			bimage.scaled = stricmp(token, "scaled") == 0;
			bimage.fit = true;
			buffer = g_pFullFileSystem->ParseFile(buffer, token, NULL);
			bimage.x = atoi(token);
			buffer = g_pFullFileSystem->ParseFile(buffer, token, NULL);
			bimage.y = atoi(token);
		}
	}
}

void CBasePanel::SetBackgroundRenderState(EBackgroundState state)
{
	float frametime = gEngfuncs.GetClientTime();
	if (state == BACKGROUND_NONE)
	{
	}
	else if (state == BACKGROUND_DESKTOPIMAGE)
	{
		m_bFadingInMenus = true;
		m_flFadeMenuStartTime = frametime;
		m_flFadeMenuEndTime = frametime + 0.2f;
	}
	m_eBackgroundState = state;
}

void CBasePanel::DrawBackgroundImage(void)
{
	int wide, tall;
	GetSize(wide, tall);

	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	float xScale = (float)swide / (float)m_iBaseResX;
	float yScale = (float)stall / (float)m_iBaseResY;

	float coverScale, coverWide, coverTall;
	if (swide > yScale * m_iBaseResX)
	{
		coverScale = xScale;
		coverWide = m_iBaseResX * xScale;
		coverTall = m_iBaseResY * xScale;
	}
	else
	{
		coverScale = yScale;
		coverWide = m_iBaseResX * yScale;
		coverTall = m_iBaseResY * yScale; // == stall
	}
	int fitOffsetX = swide / 2 - (int)(coverWide * 0.5f);
	int fitOffsetY = stall / 2 - (int)(coverTall * 0.5f);

	for (int i = 0; i < m_ImageID.Count(); i++)
	{
		bimage_t &bimage = m_ImageID[i];

		int dx, dy, dw, dt;

		if (bimage.scaled)
		{
			dx = bimage.x ? (int)ceil(bimage.x * xScale) : 0;
			dy = bimage.y ? (int)ceil(bimage.y * yScale) : 0;
			dw = (int)ceil((bimage.width + bimage.x) * xScale);
			dt = (int)ceil((bimage.height + bimage.y) * yScale);
		}
		else if (bimage.fit)
		{
			int x = bimage.x ? (int)ceil(bimage.x * coverScale) : 0;
			int y = bimage.y ? (int)ceil(bimage.y * coverScale) : 0;

			dx = fitOffsetX + x;
			dy = fitOffsetY + y;
			dw = dx + (int)ceil(bimage.width * coverScale);
			dt = dy + (int)ceil(bimage.height * coverScale);
		}
		else
		{
			dx = bimage.x ? (int)ceil((double)bimage.x) : 0;
			dy = bimage.y ? (int)ceil((double)bimage.y) : 0;
			dw = bimage.width + dx;
			dt = bimage.height + dy;
		}

		surface()->DrawSetColor(255, 255, 255, 255);
		surface()->DrawSetTexture(bimage.imageID);
		surface()->DrawTexturedRect(dx, dy, dw, dt);
	}
}

void CBasePanel::RunFrame()
{
	InvalidateLayout();
//	vgui2::GetAnimationController()->UpdateAnimations( engine->Time() );

//	UpdateBackgroundState();

//	if ( !m_bPlatformMenuInitialized )
//	{
//		// check to see if the platform is ready to load yet
//		if ( IsX360() || g_VModuleLoader.IsPlatformReady() )
//		{
//			m_bPlatformMenuInitialized = true;
//		}
//	}

	// Check to see if a pending async task has already finished
//	if ( m_pAsyncJob && !m_pAsyncJob->m_hThreadHandle )
//	{
//		m_pAsyncJob->Completed();
//		delete m_pAsyncJob;
//		m_pAsyncJob = NULL;
//	}
}
void CBasePanel::CreateGameMenu()
{
	// load settings from config file
	KeyValues *datafile = new KeyValues("GameMenu");
    // datafile->UsesEscapeSequences( true );	// VGUI uses escape sequences
	if (datafile->LoadFromFile( g_pFullFileSystem, "resource/GameMenu.res" ) )
	{
		m_pGameMenu = RecursiveLoadGameMenu(datafile);
	}

	if ( !m_pGameMenu )
	{
		Error( "Could not load file Resource/GameMenu.res" );
	}
	else
	{
		// start invisible
		SETUP_PANEL( m_pGameMenu );
		m_pGameMenu->SetAlpha( 255 );
	}

	datafile->deleteThis();
}
void CBasePanel::UpdateGameMenus()
{
	// check our current state
	bool isInGame = GameUI().IsInLevel();
	bool isMulti = isInGame && (gEngfuncs.GetMaxClients() > 1);
//	bool isInReplay = GameUI().IsInReplay();
//	bool isVREnabled = materials->GetCurrentConfigForVideoCard().m_nVRModeAdapter == materials->GetCurrentAdapter();
//	bool isVRActive = UseVR();

	// iterate all the menu items
	m_pGameMenu->UpdateMenuItemState( isInGame, isMulti, false, false, false );

//	if ( m_hMainMenuOverridePanel )
//	{
//		vgui2::ivgui()->PostMessage( m_hMainMenuOverridePanel, new KeyValues( "UpdateMenu" ), NULL );
//	}

	// position the menu
	InvalidateLayout();
	m_pGameMenu->SetVisible( true );
}
void CBasePanel::PerformLayout()
{
    // m_pGameMenu->SetPos(300, 300);
    // UpdateGameMenus();
    BaseClass::PerformLayout();
}
CGameMenu *CBasePanel::RecursiveLoadGameMenu(KeyValues *datafile)
{
	CGameMenu *menu = new CGameMenu(this, datafile->GetName());

	wchar_t *pString = g_pVGuiLocalize->Find( "#GameUI_Console" );

	if( pString )
		menu->AddMenuItem("Console", V_wcsupr(pString), "OpenConsole", this);
	else
		menu->AddMenuItem("Console", "CONSOLE", "OpenConsole", this);

	bool bFoundServerBrowser = false;

	for (KeyValues *dat = datafile->GetFirstSubKey(); dat != NULL; dat = dat->GetNextKey())
	{
		const char *label = dat->GetString("label", "<unknown>");
		const char *cmd = dat->GetString("command", NULL);
		const char *name = dat->GetString("name", label);

		if( cmd && Q_strcmp(cmd, "OpenServerBrowser") == 0 )
			bFoundServerBrowser = true;
	}

	// if( !bFoundServerBrowser && !ModInfo().IsSinglePlayerOnly() )
	// menu->AddMenuItem("AntiM*dG*yButton", "#GameUI_GameMenu_FindServers", "OpenServerBrowser", this);

	// loop through all the data adding items to the menu
	for (KeyValues *dat = datafile->GetFirstSubKey(); dat != NULL; dat = dat->GetNextKey())
	{
		const char *label = dat->GetString("label", "<unknown>");
		const char *cmd = dat->GetString("command", NULL);
		const char *name = dat->GetString("name", label);

		if ( cmd && (!Q_stricmp( cmd, "OpenFriendsDialog" )
		        || !Q_stricmp( cmd, "engine bug" )) )
			continue;

		menu->AddMenuItem(name, label, cmd, this, dat);
	}

	return menu;
}
CGameMenu::~CGameMenu() {}

void CBasePanel::OnCommand(const char* command)
{
    if (!stricmp(command, "OpenOptionsDialog"))
	{

		OnOpenOptionsDialog();
	}
	else if (!V_strcmp(command, "ConnectDialog"))
	{
		if (!m_hServerConnectDialog.Get())
		{
			m_hServerConnectDialog = new ServerConnectPanel(this);
		}
		m_hServerConnectDialog.Get()->Activate();
		PositionDialog(m_hServerConnectDialog);

		g_VModuleLoader.ActivateModule(0);
		g_VModuleLoader.ActivateModule(1);
	}
	else if (!V_strcmp(command, "AdvOptionsDialog"))
	{
		gEngfuncs.pfnClientCmd("gameui_cl_open_adv_options");
	}
	else if (!V_strcmp(command, "CreateServerDialog"))
	{
		if (!m_hCreateMultiplayerGameDialog.Get())
			m_hCreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog(this);
		m_hCreateMultiplayerGameDialog.Get()->Activate();
		PositionDialog(m_hCreateMultiplayerGameDialog);
	}
	else if (!V_strcmp(command, "NewGameDialog"))
	{
		if (!m_hNewGameDialog.Get())
			m_hNewGameDialog = new CNewGameDialog(this);
		m_hNewGameDialog.Get()->Activate();
		PositionDialog(m_hNewGameDialog);
	}
	else if (!V_strcmp(command, "LoadGameDialog"))
	{
		if (!m_hLoadGameDialog.Get())
			m_hLoadGameDialog = new CLoadGameDialog(this);
		m_hLoadGameDialog.Get()->Activate();
		PositionDialog(m_hLoadGameDialog);
	}
	else if (!V_strcmp(command, "SaveGameDialog"))
	{
		if (!m_hSaveGameDialog.Get())
			m_hSaveGameDialog = new CSaveGameDialog(this);
		m_hSaveGameDialog.Get()->Activate();
		PositionDialog(m_hSaveGameDialog);
	}

	BaseClass::OnCommand(command);
}
void CBasePanel::PositionDialog(vgui2::PHandle dlg)
{
    if (!dlg.Get())
        return;

    int x, y, ww, wt, wide, tall;
    vgui2::surface()->GetWorkspaceBounds( x, y, ww, wt );
    dlg->GetSize(wide, tall);

    // Center it, keeping requested size
    dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}
