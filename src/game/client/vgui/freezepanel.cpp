//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

// #include "cbase.h"
#include "freezepanel.h"
#include <vgui/IVGui.h>
#include "parsemsg.h"
#include "vgui_controls/AnimationController.h"
// #include "iclientmode.h"
// #include "c_cs_player.h"
// #include "c_cs_playerresource.h"
#include <vgui_controls/Label.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
// #include "VGUI/bordered_panel.h"
#include "fmtstr.h"
// #include "cs_gamerules.h"
#include "view.h"
#include "viewport_panel_names.h"
// #include "ivieweffects.h"
// #include "viewrender.h"
// #include "usermessages.h"
// #include "hud_macros.h"
// #include "c_baseanimating.h"
// #include "backgroundpanel.h"	// rounded border support

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// DECLARE_HUDELEMENT_DEPTH( CCSFreezePanel, 1 );
// DECLARE_HUD_MESSAGE( CCSFreezePanel, Damage );
// DECLARE_HUD_MESSAGE( CCSFreezePanel, DroppedEquipment );

#define CALLOUT_WIDE		(XRES(100))
#define CALLOUT_TALL		(XRES(50))

Color LerpColors( Color cStart, Color cEnd, float flPercent )
{
    float r = (float)((float)(cStart.r()) + (float)(cEnd.r() - cStart.r()) * flPercent);
    float g = (float)((float)(cStart.g()) + (float)(cEnd.g() - cStart.g()) * flPercent);
    float b = (float)((float)(cStart.b()) + (float)(cEnd.b() - cStart.b()) * flPercent);
    float a = (float)((float)(cStart.a()) + (float)(cEnd.a() - cStart.a()) * flPercent);
    return Color( r, g, b, a );
}

class HorizontalGauge : public vgui2::Panel
{
public:
    DECLARE_CLASS_SIMPLE( HorizontalGauge, vgui2::Panel );

    HorizontalGauge( Panel *parent, const char *name ) :
        vgui2::Panel( parent, name ),
        m_fPercent(0.0f)
    {
    }

    /*
    void ApplySettings(KeyValues *inResourceData)
    {
        BaseClass::ApplySettings(inResourceData);

        Color color0 = inResourceData->GetColor( "color0");
        Color color1 = inResourceData->GetColor( "color1");
    }
*/

    void PaintBackground()
    {
        int wide, tall;
        GetSize(wide, tall);

        surface()->DrawSetColor( Color(0, 0, 0, 128) );
        surface()->DrawFilledRect(0, 0, wide, tall);

        // do the border explicitly here
        surface()->DrawSetColor( Color(0,0,0,255));
        surface()->DrawOutlinedRect(0, 0, wide, tall);
    }

    virtual void Paint()
    {
        int wide, tall;
        GetSize(wide, tall);

        Color lowHealth(192, 32, 32, 255);
        Color highHealth(32, 255, 32, 255);

        surface()->DrawSetColor( LerpColors(lowHealth, highHealth, m_fPercent) );
        surface()->DrawFilledRect(1, 1, (int)((wide - 1) * m_fPercent), tall - 1);
    }

    void SetPercent( float fPercent ) { m_fPercent = fPercent; }

private:
    float m_fPercent;
};

DECLARE_BUILD_FACTORY( HorizontalGauge );

CCSFreezePanel::CCSFreezePanel() :
    EditablePanel( NULL, VIEWPORT_PANEL_FREEZEPANEL ),
    // CHudElement( pElementName ),
    m_pBackgroundPanel(NULL),
    m_pKillerHealth(NULL),
    m_pAvatar(NULL)
    // m_pDominationIcon(NULL)
{
    SetSize( 10, 10 ); // Quiet "parent not sized yet" spew
    m_bShouldBeVisible = false;
    SetScheme( "ClientScheme" );
    // RegisterForRenderGroup( "hide_for_scoreboard" );
    LoadControlSettings( "ui/resource/FreezePanel_Basic.res" );
    m_pKillerHealth = new HorizontalGauge(this, "KillerHealth");
    m_pAvatar = new ImagePanel(this, "AvatarImage"); // new CAvatarImagePanel(this, "AvatarImage");
    m_pAvatar->SetShouldScaleImage(false);
    m_pAvatarImage = new CAvatarImage();
    // m_pAvatarImage->SetShouldDrawFriendIcon(false);
    m_pAvatarImage->SetDrawFriend(false);
    m_pAvatarImage->SetSize(64, 64);
    m_pAvatar->SetSize(64, 64);
    m_pAvatar->SetImage(m_pAvatarImage);
    pi = GetPlayerInfoSafe(0);
    SetProportional(true);
}
void CCSFreezePanel::Init()
{
    // HookMessage<&CCSFreezePanel::MsgFunc_ShowPanel>("ShowFreezePanel");
    // HookMessage<&CCSFreezePanel::MsgFunc_HidePanel>("HideFreezePanel");
    // BaseHudClass::Init();
}

void CCSFreezePanel::Reset()
{
    Hide();
}
void CCSFreezePanel::ApplySchemeSettings( vgui2::IScheme *pScheme )
{
    BaseClass::ApplySchemeSettings( pScheme );
}

int CCSFreezePanel::UpdatePanelInfo(bool m_bShown, int m_iAttackerId, int m_iAttackerHP)
{
    pi = GetPlayerInfoSafe(m_iAttackerId);
    // uint64 steamID64 = pi->GetValidSteamID64();
    // if(ClientSteamContext().SteamFriends() && ClientSteamContext().SteamUtils() && steamID64 != 0)
    // {
    //     CSteamID steamIDForPlayer(steamID64);
    // }
    m_pKillerHealth->SetPercent( (float)m_iAttackerHP / 100.0f/*iMaxHealth*/ );
    SetDialogVariable( "InfoLabel1", g_pVGuiLocalize->Find("#FreezePanel_Killer1"));
    SetDialogVariable( "InfoLabel2", g_pVGuiLocalize->Find("#FreezePanel_Killer2"));

    // if (m_iAttackerId != 0 && ClientSteamContext().SteamFriends() && ClientSteamContext().SteamUtils())
    //     m_pAvatar->SetPlayer(m_iAttackerId, k_EAvatarSize64x64);
    // else
    //     m_pAvatar->ClearAvatar();
    uint64 steamId = pi->GetValidSteamID64();
    if (ClientSteamContext().SteamUtils() && steamId != 0)
    {
        CSteamID steamIDForPlayer(steamId);
        m_pAvatarImage->SetAvatarSteamID(steamIDForPlayer, k_EAvatarSize64x64);
    }
    else
    {
        m_pAvatarImage->ClearAvatarSteamID();
    }

    SetDialogVariable("killername", pi->GetDisplayName());
    // m_pKillerName->SetText(pi->GetDisplayName());
    SetVisible(m_bShown);
    return 1;
}

void CCSFreezePanel::Show()
{
    m_bShouldBeVisible = true;
}
void CCSFreezePanel::OnScreenSizeChanged( int nOldWide, int nOldTall )
{
    BaseClass::OnScreenSizeChanged(nOldWide, nOldTall);

    InitLayout();
}
void CCSFreezePanel::InitLayout()
{
#if 0
    LoadControlSettings( "resource/UI/FreezePanel_Basic.res" );

    m_pBackgroundPanel = dynamic_cast<BorderedPanel*>( FindChildByName("FreezePanelBG"));
    m_pAvatar = dynamic_cast<CAvatarImagePanel*>( m_pBackgroundPanel->FindChildByName("AvatarImage"));
    m_pKillerHealth	= dynamic_cast<HorizontalGauge*>( m_pBackgroundPanel->FindChildByName("KillerHealth"));
    m_pDominationIcon = dynamic_cast<ImagePanel*>( m_pBackgroundPanel->FindChildByName("DominationIcon"));

    m_pAvatar->SetDefaultAvatar(scheme()->GetImage( CSTRIKE_DEFAULT_AVATAR, true ));
    m_pAvatar->SetShouldScaleImage(true);
    m_pAvatar->SetShouldDrawFriendIcon(false);
#endif
}
void CCSFreezePanel::SetActive( bool bActive )
{
#if 0
    CHudElement::SetActive( bActive );

    if ( bActive )
    {
        // Setup replay key binding in UI
        const char *pKey = engine->Key_LookupBinding( "save_replay" );
        if ( pKey == NULL || FStrEq( pKey, "(null)" ) )
        {
            pKey = "<NOT BOUND>";
        }

        char szKey[16];
        Q_snprintf( szKey, sizeof(szKey), "%s", pKey );
        wchar_t wKey[16];
        wchar_t wLabel[256];

        g_pVGuiLocalize->ConvertANSIToUnicode( szKey, wKey, sizeof( wKey ) );
        g_pVGuiLocalize->ConstructString( wLabel, sizeof( wLabel ), g_pVGuiLocalize->Find("#FreezePanel_SaveReplay" ), 1, wKey );

        m_pBackgroundPanel->SetDialogVariable( "savereplay", wLabel );
    }
#endif
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCSFreezePanel::Hide()
{
    m_bShouldBeVisible = false;
}
void CCSFreezePanel::ShowPanel(bool state)
{
    if (state != IsVisible())
    {
        SetVisible(state);
    }
}

vgui2::VPANEL CCSFreezePanel::GetVPanel()
{
    return BaseClass::GetVPanel();
}

bool CCSFreezePanel::IsVisible()
{
    return BaseClass::IsVisible();
}

void CCSFreezePanel::SetParent(vgui2::VPANEL parent)
{
    BaseClass::SetParent(parent);
}
const char *CCSFreezePanel::GetName()
{
    return VIEWPORT_PANEL_FREEZEPANEL;
}
