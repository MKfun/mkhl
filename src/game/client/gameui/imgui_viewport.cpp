//
// Created by Elinsrc on 30.11.25.
//
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "imgui_viewport.h"
#include "ui_demo_window.h"
#include <string.h>
extern CImGuiDemoWindow iDemoWindow;
// Team Colors
int iNumberOfTeamColors = 5;
int iTeamColors[5][3] =
    {
	    { 255, 170, 0 },	// HL orange (default)
	    { 125, 165, 210 },	// Blue
	    { 200, 90, 70 },	// Red
	    { 225, 205, 45 },	// Yellow
	    { 145, 215, 140 },	// Green
    };

CImGuiViewport &g_ImGuiViewport = CImGuiViewport::GetInstance();

CImGuiViewport &CImGuiViewport::GetInstance()
{
	static CImGuiViewport instance;
	return instance;
}

CImGuiViewport::CImGuiViewport()
{
}

CImGuiViewport::~CImGuiViewport()
{
}

void CImGuiViewport::Initialize()
{
	m_iGotAllMOTD = true;
	m_szServerName[0] = '\0';
}

