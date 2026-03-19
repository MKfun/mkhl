//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#include "freezepanel.h"
#include <vgui/IVGui.h>
#include "parsemsg.h"
#include "vgui_controls/AnimationController.h"
#include <vgui_controls/Label.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include "fmtstr.h"
#include "view.h"
#include "viewport_panel_names.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


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
    m_pBackgroundPanel(NULL),
    m_pKillerHealth(NULL),
    m_pAvatar(NULL)
    // m_pDominationIcon(NULL)
{
    SetSize( 10, 10 ); // Quiet "parent not sized yet" spew
    SetProportional(true);
    m_bShouldBeVisible = false;
    m_pKillerHealth = new HorizontalGauge(this, "KillerHealth");
    m_pAvatar = new ImagePanel(this, "AvatarImage"); // new CAvatarImagePanel(this, "AvatarImage");
    LoadControlSettings( "ui/resource/FreezePanel_Basic.res" );
    m_pAvatar->SetShouldScaleImage(false);
    m_pAvatarImage = new CAvatarImage();
    m_pAvatarImage->SetDrawFriend(false);
    int x, y, wide, tall = 0;
    m_pAvatar->GetBounds(x, y, wide, tall);
    m_pAvatar->SetShouldScaleImage(0);
    m_pAvatarImage->SetSize(m_pAvatar->GetWide(), m_pAvatar->GetTall());
    m_pAvatar->SetImage(m_pAvatarImage);

    pi = GetPlayerInfoSafe(0);
}
void CCSFreezePanel::Init()
{
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
    m_pKillerHealth->SetPercent(clamp( m_iAttackerHP / 100.0f, 0.0f, 1.0f ));
    SetDialogVariable( "InfoLabel1", g_pVGuiLocalize->Find("#FreezePanel_Killer1"));
    SetDialogVariable( "InfoLabel2", g_pVGuiLocalize->Find("#FreezePanel_Killer2"));
    uint64 steamId = pi->GetValidSteamID64();
    if (ClientSteamContext().SteamUtils() && steamId != 0)
    {
        CSteamID steamIDForPlayer(steamId);
        m_pAvatarImage->SetAvatarSteamID(steamIDForPlayer, k_EAvatarSize184x184);
    }
    else
    {
        m_pAvatarImage->ClearAvatarSteamID();
    }

    SetDialogVariable("killername", pi->GetDisplayName());
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
