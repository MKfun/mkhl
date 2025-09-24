#include <vgui/IPanel.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Label.h>
#include "hud.h"
#include "cl_util.h"
#include "client_steam_context.h"
#include "client_vgui.h"
#include "client_viewport.h"
// #include "player.h"
#include "vgui_controls/ProgressBar.h"
#include "parsemsg.h"
#include "eventscripts.h"
#include <vgui_controls/EditablePanel.h>
#include "global_consts.h"
#include "IViewportPanel.h"
#include "vgui_controls/ProgressBar.h"
#include "viewport_panel_names.h"

namespace vgui2
{
    class Label;
}

class CHudGauss : public vgui2::EditablePanel, public IViewportPanel
{
public:
    DECLARE_CLASS_SIMPLE(CHudGauss, vgui2::EditablePanel);

    CHudGauss();
    virtual void ApplySchemeSettings(vgui2::IScheme *pScheme) override;
    virtual void PaintBackground() override;

    void UpdateGauss(float gaussamnt);

    void OnThink() override;

    //IViewportPanel overrides
    virtual const char *GetName() override;
    virtual void Reset() override;
    virtual void ShowPanel(bool state) override;
    virtual vgui2::VPANEL GetVPanel() override;
    virtual bool IsVisible() override;
    virtual void SetParent(vgui2::VPANEL parent) override;

private:
    vgui2::Label *m_pGaussAmount = nullptr;
    vgui2::ContinuousProgressBar *m_pGaussProgress = nullptr;
    float m_fFade = 0.0f;
    int m_flGauss = 100;
    float m_flChargeStartTime; // Время начала заряда
    bool m_bIsCharging;       // Флаг состояния заряда

    ConVarRef m_pHudDim{"hud_dim"};
};
ConVar hud_gauss("hud_gauss", "0", FCVAR_ARCHIVE, "gauss amount to zap");
extern cl_entity_t *g_localPlayer;  // Declare if not already in your headers

CHudGauss::CHudGauss()
: BaseClass(nullptr, VIEWPORT_PANEL_HUD_HEALTH)
{
    // HOOK_MESSAGE(GaussSpin);
    m_flGauss = 0;
    m_flChargeStartTime = 0;
    m_bIsCharging = false;

    SetSize(100, 100); // Silence "parent not sized yet" warning
    SetProportional(true);
    m_pGaussAmount = new vgui2::Label(this, "GaussAmount", "0");
    m_pGaussProgress = new vgui2::ContinuousProgressBar( this, "GaussAmountProgress");
    LoadControlSettings(VGUI2_ROOT_DIR "resource/HudGauss.res");


    SetVisible(false);
}

void CHudGauss::ApplySchemeSettings(vgui2::IScheme *pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);
    SetPaintBackgroundEnabled(true);
}

void CHudGauss::PaintBackground()
{
    DrawBox(0, 0, GetWide(), GetTall(), GetBgColor(), 1.0f);
}

void CHudGauss::UpdateGauss(float gaussamnt)
{

    if (gaussamnt != m_flGauss)
        m_fFade = FADE_TIME;

    m_flGauss = gaussamnt;

    // Update health amount text
    char buf[32];
    // snprintf(buf, sizeof(buf), "%d", gaussamnt);
    m_pGaussProgress->SetProgress( clamp( gaussamnt / 10.0f, 0.0f, 1.0f ));
}

const char *CHudGauss::GetName()
{
    return VIEWPORT_PANEL_HUD_HEALTH;
}

void CHudGauss::Reset()
{
}

void CHudGauss::OnThink()
{
;
    cl_entity_t *pLocal = gEngfuncs.GetLocalPlayer();
    if (!pLocal) return;

    // Определяем состояние заряда по анимации оружия
    bool bIsChargingNow = (pLocal->curstate.weaponanim == 6);

    if (bIsChargingNow && !m_bIsCharging)
    {
        // Начало заряда
        m_flChargeStartTime = gEngfuncs.GetClientTime();
        m_bIsCharging = true;
    }
    else if (!bIsChargingNow && m_bIsCharging)
    {
        // Конец заряда
        m_bIsCharging = false;
        m_flGauss = 0;
    }

    // Расчет текущего заряда
    if (m_bIsCharging)
    {
        float flChargeTime = gEngfuncs.GetClientTime() - m_flChargeStartTime;

        // Гаусс заряжается 3 секунды до полного заряда
        m_flGauss = flChargeTime / 3.0f;

        // Ограничение 0-1
        if (m_flGauss > 1.0f)
            m_flGauss = 1.0f;
        else if (m_flGauss < 0.0f)
            m_flGauss = 0.0f;
    }

        UpdateGauss(m_flGauss);
    // Glow effect will be visible only when dimmed
    auto [a1, a2] = gHUD.GetHudDimAlphas(m_pHudDim.GetBool(), m_fFade, gHUD.m_flTimeDelta);

    // Color currentColor = gHUD.GetHudColor(HudPart::Health, m_iHealth);
    // currentColor[3] = a1;
    // m_pGaussProgress->SetFgColor(currentColor);
    //
    // currentColor[3] = a2;
    // m_pGaussProgress->SetFgColor(currentColor);
    // m_pGaussProgress->SetFgColor(currentColor);
    if (!hud_gauss.GetBool())
    {
        SetVisible(false);
    }
    else {
        SetVisible(true);
    }
}

void CHudGauss::ShowPanel(bool state)
{
    if (state != IsVisible())
    {
        SetVisible(state);
    }
}

vgui2::VPANEL CHudGauss::GetVPanel()
{
    return BaseClass::GetVPanel();
}

bool CHudGauss::IsVisible()
{
    return BaseClass::IsVisible();
}

void CHudGauss::SetParent(vgui2::VPANEL parent)
{
    BaseClass::SetParent(parent);
}
