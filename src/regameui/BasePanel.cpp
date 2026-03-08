//
// Created by den4ik on 01.12.25.
//
#include <stdio.h>

#include "BasePanel.h"
#include "EngineInterface.h"
#include "IEngineVGui.h"
#include "GameUI_Interface.h"
#include "GameMenu.h"
#include "ModInfo.h"

#include "BackgroundMenuButon.h"
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include <vgui/ILocalize.h>
extern cl_enginefunc_t gEngfuncs;
using namespace vgui2;
static CBasePanel	*g_pBasePanel = NULL;

CBasePanel *BasePanel()
{
	return g_pBasePanel;
}
static CBackgroundMenuButton* CreateMenuButton( CBasePanel *parent, const char *panelName, const wchar_t *panelText )
{
	CBackgroundMenuButton *pButton = new CBackgroundMenuButton( parent, panelName );
	pButton->SetCommand("OpenGameMenu");
	pButton->SetText(panelText);

	return pButton;
}
//-----------------------------------------------------------------------------
CBasePanel::CBasePanel() : EditablePanel(NULL, "BaseGameUIPanel")
{
	SetProportional( true );
	g_pBasePanel = this;
	m_eBackgroundState = BACKGROUND_BLACK;
	m_pButton = new Button(this, "button", "button1");
	m_pButton->SetArmed(true);
	m_pButton->SetVisible(true);
	m_pButton->SetPos(200, 200);
//	m_pGameMenu = NULL;

	m_pGameMenuButtons.AddToTail( CreateMenuButton( this, "GameMenuButton", ModInfo().GetGameTitle() ) );
	m_pGameMenuButtons.AddToTail( CreateMenuButton( this, "GameMenuButton2", ModInfo().GetGameTitle2() ) );	CreateGameMenu();
	LoadControlSettings("resource/mainmenu.res");
}

void CBasePanel::OnChildAdded(VPANEL child)
{

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

	for (int y = 0; y < BACKGROUND_ROWS; y++)
	{
		for (int x = 0; x < BACKGROUND_COLUMNS; x++)
		{
			bimage_t &bimage = m_ImageID[y][x];
			bimage.imageID = surface()->CreateNewTextureID();

			char filename[512];
			sprintf(filename, "resource/background/800_%d_%c_loading", y + 1, 'a' + x);
			surface()->DrawSetTextureFile(bimage.imageID, filename, hardwareFilter, false);
			surface()->DrawGetTextureSize(bimage.imageID, bimage.width, bimage.height);
		}
	}
}

void CBasePanel::SetBackgroundRenderState(EBackgroundState state)
{
	m_eBackgroundState = state;
}

void CBasePanel::DrawBackgroundImage()
{
		int xpos, ypos;
	int wide, tall;
	GetSize(wide, tall);

	// work out scaling factors
	int swide, stall;
	surface()->GetScreenSize(swide, stall);
	float xScale, yScale;
		xScale = swide / 800.0f;
		yScale = stall / 600.0f;
	xScale = 1.0f;
	yScale = 1.0f;

	// iterate and draw all the background pieces
	ypos = 0;
	for (int y = 0; y < BACKGROUND_ROWS; y++)
	{
		xpos = 0;
		for (int x = 0; x < BACKGROUND_COLUMNS; x++)
		{
			bimage_t &bimage = m_ImageID[y][x];

			int dx = (int)ceil(xpos * xScale);
			int dy = (int)ceil(ypos * yScale);
			int dw = (int)ceil((xpos + bimage.width) * xScale);
			int dt = (int)ceil((ypos + bimage.height) * yScale);

			if (x == 0)
			{
				dx = 0;
			}
			if (y == 0)
			{
				dy = 0;
			}

//	bimage_t &bimage = m_ImageID[0][0];
	// draw the color image only if the mono image isn't yet fully opaque
//	surface()->DrawSetColor(255, 255, 255, 255);
//	surface()->DrawSetTexture(bimage.imageID);
//	surface()->DrawTexturedRect(0, 0, wide, tall);

		xpos += bimage.width;
		}
		ypos += m_ImageID[y][0].height;
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
//	datafile->UsesEscapeSequences( true );	// VGUI uses escape sequences
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
	m_pGameMenu->SetPos(300, 300);
	UpdateGameMenus();
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

	if( !bFoundServerBrowser && !ModInfo().IsSinglePlayerOnly() )
		menu->AddMenuItem("AntiM*dG*yButton", "#GameUI_GameMenu_FindServers", "OpenServerBrowser", this);

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
