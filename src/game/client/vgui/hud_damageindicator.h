#ifndef HUD_DAMAGEINDICATOR_H
#define HUD_DAMAGEINDICATOR_H

#include "vgui/IViewportPanel.h"
#include "vgui/avatar_image.h"
#include "vgui/tga_image.h"
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

class CHudDamageIndicator : public vgui2::Panel, public IViewportPanel
{
    DECLARE_CLASS_SIMPLE( CHudDamageIndicator, vgui2::Panel );
public:
    CHudDamageIndicator();
    void Init( void );
    void VidInit( void );
    void Reset( void );
    virtual bool ShouldDraw( void );

    // Handler for our message
    void UpdateDamagePanel(int armor, int damageTaken, int bitsDamage, Vector vecFrom);

    //IViewportPanel overrides
    virtual const char *GetName() override {return "HudDamageIndicator"; };
    // virtual void Reset() override;
    virtual void ShowPanel(bool state) override;
    virtual vgui2::VPANEL GetVPanel() override;
    virtual bool IsVisible() override;
    virtual void SetParent(vgui2::VPANEL parent) override;
private:
    virtual void OnThink();
    virtual void Paint();
    virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

    // Painting
    void GetDamagePosition( const Vector &vecDelta, float flRadius, float *xpos, float *ypos, float *flRotation );
    void DrawDamageIndicator(int x0, int y0, int x1, int y1, float alpha, float flRotation );
private:
    // Indication times

    CPanelAnimationVarAliasType( float, m_flMinimumWidth, "MinimumWidth", "10", "proportional_float" );
    CPanelAnimationVarAliasType( float, m_flMaximumWidth, "MaximumWidth", "100", "proportional_float" );
    CPanelAnimationVarAliasType( float, m_flMinimumHeight, "MinimumHeight", "20", "proportional_float" );
    CPanelAnimationVarAliasType( float, m_flMaximumHeight, "MaximumHeight", "100", "proportional_float" );
    CPanelAnimationVarAliasType( float, m_flStartRadius, "StartRadius", "140", "proportional_float" );
    CPanelAnimationVarAliasType( float, m_flEndRadius, "EndRadius", "120", "proportional_float" );

    CPanelAnimationVar( float, m_iMaximumDamage, "MaximumDamage", "100" );
    CPanelAnimationVar( float, m_flMinimumTime, "MinimumTime", "1" );
    CPanelAnimationVar( float, m_flMaximumTime, "MaximumTime", "2" );
    CPanelAnimationVar( float, m_flTravelTime, "TravelTime", ".1" );
    CPanelAnimationVar( float, m_flFadeOutPercentage, "FadeOutPercentage", "0.7" );
    CPanelAnimationVar( float, m_flNoise, "Noise", "0.1" );

    // List of damages we've taken
    struct damage_t
    {
        int		iScale;
        float	flLifeTime;
        float	flStartTime;
        Vector	vecDelta;	// Damage origin relative to the player
    };
    CUtlVector<damage_t>	m_vecDamages;
    CTGAImage m_WhiteAdditiveMaterial;
};
#endif // HUD_DAMAGEINDICATOR_H
