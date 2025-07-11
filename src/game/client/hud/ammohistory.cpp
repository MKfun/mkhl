/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
//  ammohistory.cpp
//

#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"

#include <string.h>
#include <stdio.h>

#include "ammohistory.h"
#include "vgui/client_viewport.h"

ConVar hud_drawhistory_time("hud_drawhistory_time", "5", FCVAR_BHL_ARCHIVE, "How long should ammo history stay up for in seconds");

HistoryResource gHR;

// Allow to change ammo history position when using custom HUD to avoid overlapping
extern ConVar hud_custom;
#define AMMO_PICKUP_GAP         (gHR.iHistoryGap + 5)
#define AMMO_PICKUP_PICK_HEIGHT (((hud_custom.GetBool()) ? ScreenHeight - g_pViewport->GetAmmoHistoryYPos() : 32) + (gHR.iHistoryGap * 2))
#define AMMO_PICKUP_HEIGHT_MAX  (ScreenHeight - 100)

#define MAX_ITEM_NAME 32
int HISTORY_DRAW_TIME = 5;

// keep a list of items
struct ITEM_INFO
{
	char szName[MAX_ITEM_NAME];
	HSPRITE spr;
	wrect_t rect;
};

void HistoryResource::AddToHistory(int iType, int iId, int iCount)
{
	if (iType == HISTSLOT_AMMO && !iCount)
		return; // no amount, so don't add

	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY))
	{ // the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++]; // default to just writing to the first slot
	HISTORY_DRAW_TIME = hud_drawhistory_time.GetInt();

	freeslot->type = iType;
	freeslot->iId = iId;
	freeslot->iCount = iCount;
	freeslot->DisplayTime = gHUD.m_flTime + HISTORY_DRAW_TIME;
}

void HistoryResource::AddToHistory(int iType, const char *szName, int iCount)
{
	if (iType != HISTSLOT_ITEM)
		return;

	if ((((AMMO_PICKUP_GAP * iCurrentHistorySlot) + AMMO_PICKUP_PICK_HEIGHT) > AMMO_PICKUP_HEIGHT_MAX) || (iCurrentHistorySlot >= MAX_HISTORY))
	{ // the pic would have to be drawn too high
		// so start from the bottom
		iCurrentHistorySlot = 0;
	}

	HIST_ITEM *freeslot = &rgAmmoHistory[iCurrentHistorySlot++]; // default to just writing to the first slot

	// I am really unhappy with all the code in this file

	int i = gHUD.GetSpriteIndex(szName);
	if (i == -1)
		return; // unknown sprite name, don't add it to history

	freeslot->iId = i;
	freeslot->type = iType;
	freeslot->iCount = iCount;

	HISTORY_DRAW_TIME = hud_drawhistory_time.GetInt();
	freeslot->DisplayTime = gHUD.m_flTime + HISTORY_DRAW_TIME;
}

void HistoryResource::CheckClearHistory(void)
{
	for (int i = 0; i < MAX_HISTORY; i++)
	{
		if (rgAmmoHistory[i].type)
			return;
	}

	iCurrentHistorySlot = 0;
}

//
// Draw Ammo pickup history
//
int HistoryResource::DrawAmmoHistory(float flTime)
{
	//! Margin from the right screen border to sprite center
	const int itemMarginRight = SPR_RES_SCALED(24);
	const int weaponMarginRight = SPR_RES_SCALED(16);
	const int itemWidth = SPR_RES_SCALED(16);

	for (int i = 0; i < MAX_HISTORY; i++)
	{
		if (rgAmmoHistory[i].type)
		{
			rgAmmoHistory[i].DisplayTime = min(rgAmmoHistory[i].DisplayTime, gHUD.m_flTime + HISTORY_DRAW_TIME);

			if (rgAmmoHistory[i].DisplayTime <= flTime)
			{ // pic drawing time has expired
				memset(&rgAmmoHistory[i], 0, sizeof(HIST_ITEM));
				CheckClearHistory();
			}
			else if (rgAmmoHistory[i].type == HISTSLOT_AMMO)
			{
				wrect_t rcPic;
				HSPRITE *spr = gWR.GetAmmoPicFromWeapon(rgAmmoHistory[i].iId, rcPic);

				int r, g, b;
				float a;

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				a = min(scale, 255.f) * gHUD.GetHudTransparency();

				gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
				ScaleColors(r, g, b, a);

				// Draw the pic
				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - rcPic.GetWidth() - SPR_RES_SCALED(4);
				if (spr && *spr) // weapon isn't loaded yet so just don't draw the pic
				{ // the dll has to make sure it has sent info the weapons you need
					SPR_Set(*spr, r, g, b);
					SPR_DrawAdditive(0, xpos - rcPic.GetWidth() / 2, ypos, &rcPic);
				}

				// Draw the number
				int yposText = ypos + rcPic.GetHeight() / 2 - gHUD.GetHudFontSize() / 2;
				gHUD.DrawHudNumberString(xpos - itemWidth, yposText, xpos - 100, rgAmmoHistory[i].iCount, r, g, b);
			}
			else if (rgAmmoHistory[i].type == HISTSLOT_WEAP)
			{
				WEAPON *weap = gWR.GetWeapon(rgAmmoHistory[i].iId);

				if (!weap)
					return 1; // we don't know about the weapon yet, so don't draw anything

				int r, g, b;
				float a;

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				a = min(scale, 255.f) * gHUD.GetHudTransparency();

				gHUD.GetHudColor(HudPart::Common, 0, r, g, b);

				if (!gWR.HasAmmo(weap))
					UnpackRGB(r, g, b, RGB_REDISH); // if the weapon doesn't have ammo, display it as red

				ScaleColors(r, g, b, a);

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - weap->rcInactive.GetWidth() - weaponMarginRight;
				SPR_Set(weap->hInactive, r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &weap->rcInactive);
			}
			else if (rgAmmoHistory[i].type == HISTSLOT_ITEM)
			{
				int r, g, b;
				float a;

				if (!rgAmmoHistory[i].iId)
					continue; // sprite not loaded

				wrect_t rect = gHUD.GetSpriteRect(rgAmmoHistory[i].iId);

				float scale = (rgAmmoHistory[i].DisplayTime - flTime) * 80;
				a = min(scale, 255.f) * gHUD.GetHudTransparency();

				gHUD.GetHudColor(HudPart::Common, 0, r, g, b);
				ScaleColors(r, g, b, a);

				int ypos = ScreenHeight - (AMMO_PICKUP_PICK_HEIGHT + (AMMO_PICKUP_GAP * i));
				int xpos = ScreenWidth - rect.GetWidth() / 2 - itemMarginRight;

				SPR_Set(gHUD.GetSprite(rgAmmoHistory[i].iId), r, g, b);
				SPR_DrawAdditive(0, xpos, ypos, &rect);
			}
		}
	}

	return 1;
}
