//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================

#ifndef CS_HUD_FREEZEPANEL_H
#define CS_HUD_FREEZEPANEL_H
#include "vgui/IViewportPanel.h"
#include "vgui/avatar_image.h"
#ifdef _WIN32
#pragma once
#endif

#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ProgressBar.h>
#include "vgui/ILocalize.h"
#include "vgui/avatar_image.h"
#include "hud.h"
#include "cl_util.h"
#include "parsemsg.h"
#include "client_steam_context.h"
// #include "cs_hud_playerhealth.h"

// #include "cs_shareddefs.h"

using namespace vgui2;

class HorizontalGauge;
class BorderedPanel;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CCSFreezePanel : public EditablePanel, public IViewportPanel
{
private:
    DECLARE_CLASS_SIMPLE( CCSFreezePanel, EditablePanel );

public:
    CCSFreezePanel();

    virtual void Reset();
    virtual void Init();

    virtual void ApplySchemeSettings( vgui2::IScheme *pScheme );
    // virtual void FireGameEvent( IGameEvent * event );
    // virtual bool ShouldDraw();
    virtual void OnScreenSizeChanged(int nOldWide, int nOldTall);

    virtual void SetActive( bool bActive );
    virtual const char *GetName() override;
    virtual void ShowPanel(bool state) override;
    virtual vgui2::VPANEL GetVPanel() override;
    virtual bool IsVisible() override;
    virtual void SetParent(vgui2::VPANEL parent) override;
    void InitLayout();
    void Show();
    void Hide();
    int UpdatePanelInfo(bool m_bShown, int m_iAttackerId, int m_iAttackerHP);
    // int	HudElementKeyInput( int down, ButtonCode_t keynum, const char *pszCurrentBinding );

protected:

private:
    // struct PlayerData
    // {
    //     CPlayerInfo *pi = nullptr;
    //     CAvatarImage *pInfoAvatar = nullptr;
    // };
    Panel*			m_pBackgroundPanel;
    HorizontalGauge*		m_pKillerHealth;
    ImagePanel*		m_pAvatar;
    CAvatarImage* m_pAvatarImage;
    // ImagePanel*				m_pDominationIcon;
    CPlayerInfo *pi = nullptr;
    bool					m_bShouldBeVisible;
};

#endif //CS_HUD_FREEZEPANEL_H
