//
// Created by den4ik on 01.12.25.
//
#include <stdio.h>

#include "BasePanel.h"
#include "EngineInterface.h"
#include "IEngineVGui.h"
#include "GameUI_Interface.h"

#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>

using namespace vgui2;
static CBasePanel	*g_pBasePanel = NULL;

CBasePanel *BasePanel()
{
	return g_pBasePanel;
}

//-----------------------------------------------------------------------------
CBasePanel::CBasePanel() : EditablePanel(NULL, "BaseGameUIPanel")
{
	SetProportional( true );
	g_pBasePanel = this;
	m_eBackgroundState = BACKGROUND_NONE;
	m_pButton = new Button(this, "button", "button1");
	m_pButton->SetArmed(true);
	m_pButton->SetVisible(true);
	m_pButton->SetPos(200, 200);
	LoadControlSettings("resource/mainmenu.res");
}

void CBasePanel::OnChildAdded(VPANEL child)
{

}

void CBasePanel::PaintBackground()
{
//	const char *levelName = engine->pfnGetLevelName();
//	if (levelName && levelName[0])
//	{
		// render filled background in game
//		int swide, stall;
//		surface()->GetScreenSize(swide, stall);
//		surface()->DrawSetColor(0, 0, 0, 128);
//		surface()->DrawFilledRect(0, 0, swide, stall);
//		return;
//	}

	switch (m_eBackgroundState)
	{
	case BACKGROUND_BLACK:
	{
		// if the loading dialog is visible, draw the background black
		int swide, stall;
		surface()->GetScreenSize(swide, stall);
		surface()->DrawSetColor(0, 0, 0, 255);
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
