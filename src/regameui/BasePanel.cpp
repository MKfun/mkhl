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
    vgui2::Menu *pGameMenu = new vgui2::Menu(m_pMenuBar, "FileMenu");
    m_pMenuBar->AddMenu("Game", pGameMenu);
    pGameMenu->AddMenuItem("Create a server", new KeyValues("NewGameDialog"), pGameMenu);
	pGameMenu->AddMenuItem("Connect to a server", "ConnectDialog", this);
	vgui2::Menu *pOptionsMenu = new vgui2::Menu(m_pMenuBar, "OptionsMenu");
	m_pMenuBar->AddMenu("Options", pOptionsMenu);
	pOptionsMenu->AddMenuItem("Multiplayer options", new KeyValues("OpenOptionsDialog"), pOptionsMenu);
	pOptionsMenu->AddMenuItem("Video settings", "OpenOptionsDialog", this);
	pOptionsMenu->AddMenuItem("Audio settings", "OpenOptionsDialog", this);
	pOptionsMenu->AddMenuItem("Addon settings", "OpenOptionsDialog", this);
	// m_hOptionsDialog = new COptionsDialog(this);
	// m_hOptionsDialog->Activate();
	// OnOpenOptionsDialog();
	vgui2::Menu *pAdvOptionsMenu = new vgui2::Menu(m_pMenuBar, "AdvOptionsMenu");
	m_pMenuBar->AddMenu("Advanced Options", pAdvOptionsMenu);
	pAdvOptionsMenu->AddMenuItem("Addon settings", new KeyValues("AdvOptionsDialog"), pAdvOptionsMenu);
	SetBackgroundRenderState(BACKGROUND_DESKTOPIMAGE);
	vgui2::Frame *frame = new vgui2::Frame(this, "");
	frame->Activate();
	frame->SetAlpha(0);
	frame->SetVisible(0);
	frame->SetSize(1, 1);
}
void CBasePanel::OnOpenOptionsDialog()
{
	if (1 || !m_hOptionsDialog)
	{
		m_hOptionsDialog = new COptionsDialog(this);
	}
	m_hOptionsDialog->Activate();
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

	//int vid_level;
	//gameuifuncs->GetCurrentRenderer(NULL, 0, NULL, NULL, NULL, &vid_level);

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
	int swide, stall;
	vgui2::surface()->GetScreenSize(swide, stall);

	int wide, tall;
	GetSize(wide, tall);

	float frametime = gEngfuncs.GetAbsoluteTime();
	int alpha = 255;

	if (m_bRenderingBackgroundTransition)
	{
		alpha = (m_flTransitionEndTime - frametime) / (m_flTransitionEndTime - m_flTransitionStartTime) * 255;
		alpha = clamp(alpha, 0, 255);
	}

	int ypos = 0;

	float xScale, yScale;
	xScale = (float)swide / (float)m_iBaseResX;
	yScale = (float)stall / (float)m_iBaseResY;

	// iterate and draw all the background pieces
	for (int x = 0; x < m_ImageID.Size(); x++)
	{
		bimage_t &bimage = m_ImageID[x];

		int dx = bimage.x;
		int dy = bimage.y;
		int dw = bimage.x + bimage.width;
		int dt = bimage.y + bimage.height;

		if (bimage.scaled)
		{
			dx = (int)ceil(dx * xScale);
			dy = (int)ceil(dy * yScale);
			dw = (int)ceil(dw * xScale);
			dt = (int)ceil(dt * yScale);
		}

		// draw the color image only if the mono image isn't yet fully opaque
		vgui2::surface()->DrawSetColor(255, 255, 255, 255);
		vgui2::surface()->DrawSetTexture(bimage.imageID);
		vgui2::surface()->DrawTexturedRect(dx, dy, dw, dt);
	}

	if (IsPC() && (m_bRenderingBackgroundTransition || m_eBackgroundState == BACKGROUND_LOADING))
	{
		// if (m_pGameMenu->GetAlpha() < 255)
		// {
		// 	vgui2::surface()->DrawSetColor(255, 255, 255, alpha);
		// 	vgui2::surface()->DrawSetTexture(m_iLoadingImageID);

		// 	int twide, ttall;
		// 	vgui2::surface()->DrawGetTextureSize(m_iLoadingImageID, twide, ttall);
		// 	vgui2::surface()->DrawTexturedRect(wide - twide, tall - ttall, wide, tall);
		// }
	}

	if (m_bFadingInMenus)
	{
		alpha = (frametime - m_flFadeMenuStartTime) / (m_flFadeMenuEndTime - m_flFadeMenuStartTime) * 255;
		alpha = clamp(alpha, 0, 255);

		for (int i = 0; i < m_pGameMenuButtons.Count(); ++i)
			m_pGameMenuButtons[i]->SetAlpha(alpha);

		if (alpha == 255)
			m_bFadingInMenus = false;

		// m_pGameMenu->SetAlpha(alpha);
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
	if (!V_strcmp(command, "ConnectDialog"))
	{
		if (1 || !m_hServerConnectDialog)
		{
			m_hServerConnectDialog = new ServerConnectPanel(this);
		}
		m_hServerConnectDialog->Activate();

		g_VModuleLoader.ActivateModule(0);
		g_VModuleLoader.ActivateModule(1);
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
