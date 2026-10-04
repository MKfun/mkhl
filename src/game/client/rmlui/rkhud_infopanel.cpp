#include "rkhud_infopanel.h"

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#pragma pop_macro("Assert")

#include <convar.h>
#include "hud.h"
#include "rocketsystem.h"

ConVar rocket_hud_damage_hold_time("rocket_hud_damage_hold_time", "0.35", FCVAR_ARCHIVE, "Time in seconds before HP damage bar starts draining");
ConVar rocket_hud_damage_drain_speed("rocket_hud_damage_drain_speed", "4.0", FCVAR_ARCHIVE, "Speed factor for HP damage bar draining");

documentReloadFuncs docReloadFuncs;

RkHudInfoBar RkHudInfoBar::m_Instance = RkHudInfoBar("hud_infopanel");
RkHudInfoBar::InfoBarData RkHudInfoBar::infoBarData = {
	100, 0, false, 0, 0, 0,
	"0", "0", "0", "0", false, 255, 255, 255, 0
};

void UnloadRkInfoBar()
{
    RkHudInfoBar &pInfoBar = RkHudInfoBar::m_Instance;
	if (!pInfoBar.m_pInstance)
	{
        Warning( "Couldn't grab RkHudInfoBar element to unload!\n");
        return;
    }


    Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
    if( hudCtx )
    {
        hudCtx->RemoveDataModel("infobar_model");
		pInfoBar.m_dataModel = nullptr;
	}
    else
    {
        Warning("Couldn't access hudCtx to unload infobar datamodel\n");
	}
	// pInfoBar.m_dataModel = nullptr;

	// if (pInfoBar.m_pInstance)
	{
		pInfoBar.m_pInstance->Close();
		pInfoBar.m_pInstance = nullptr;
	}
}
void LoadRkInfoBar()
{
    docReloadFuncs.LoadDocument = &LoadRkInfoBar;
    docReloadFuncs.UnloadDocument = &UnloadRkInfoBar;

    RkHudInfoBar &pInfoBar = RkHudInfoBar::m_Instance;

    Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
    if( !hudCtx )
    {
        Error("Couldn't access hudctx!\n");
        return;
    }

	if (pInfoBar.m_pInstance || pInfoBar.m_dataModel)
	{
		Warning("RkInfoBar already loaded, call unload first!\n");
		return;
	}

	Rml::DataModelConstructor constructor = hudCtx->CreateDataModel("infobar_model");
	if (!constructor)
	{
		Error("Couldn't create datamodel for infobar!\n");
		return;
	}

	constructor.Bind("hp", &RkHudInfoBar::infoBarData.hp);
	constructor.Bind("armor", &RkHudInfoBar::infoBarData.armor);
	constructor.Bind("ammo", &RkHudInfoBar::infoBarData.ammo);
	constructor.Bind("ammo_reserve", &RkHudInfoBar::infoBarData.ammoReserve);
	constructor.Bind("fire_mode_string", &RkHudInfoBar::infoBarData.fireModeString);
	constructor.Bind("has_helmet", &RkHudInfoBar::infoBarData.hasHelmet);
	constructor.Bind("primary_string", &RkHudInfoBar::infoBarData.primaryString);
    constructor.Bind("secondary_string", &RkHudInfoBar::infoBarData.secondaryString);
    constructor.Bind("knife_string", &RkHudInfoBar::infoBarData.knifeString);
	constructor.Bind("col_r", &RkHudInfoBar::infoBarData.col_r);
	constructor.Bind("col_g", &RkHudInfoBar::infoBarData.col_g);
	constructor.Bind("col_b", &RkHudInfoBar::infoBarData.col_b);
	constructor.Bind("has_ammo_reserve", &RkHudInfoBar::infoBarData.hasSecondary);
	constructor.Bind("num_kills", &RkHudInfoBar::infoBarData.numKills);
	constructor.Bind("ammo_secondary", &RkHudInfoBar::infoBarData.ammoSecondary);

	pInfoBar.m_dataModel = constructor.GetModelHandle();

	pInfoBar.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud("body", "GAME", "rocketui/hud_infobar.rml", &docReloadFuncs);

	if (!pInfoBar.m_pInstance)
	{
		Error("Couldn't create hud_infobar document!\n");
		return;
	}
	pInfoBar.SetActive(1);
	pInfoBar.m_pInstance->Show();
	pInfoBar.m_pInstance->PullToFront();
}

RkHudInfoBar::RkHudInfoBar(const char *value)
	: m_bVisible(false)
	, m_flLastTime(0.0f)
{
    m_Instance = *this;
}

RkHudInfoBar::~RkHudInfoBar() noexcept
{
}
CON_COMMAND(rocket_dispatch_killanim, "")
{
	RkHudInfoBar::m_Instance.DispatchKillAnimation();
}
void RkHudInfoBar::DispatchKillAnimation()
{
	if (Rml::Element *element = m_pInstance->GetElementById("killAnim"))
	{
		element->SetClass("kill-anim", false);
		element->SetClass("kill-anim", true);
	}
}
void RkHudInfoBar::LevelInit()
{
    m_flLastTime = 0.0f;

    void LoadRkInfoBar();
    LoadRkInfoBar();
}

void RkHudInfoBar::LevelShutdown()
{
    m_flLastTime = 0.0f;

    void UnloadRkInfoBar();
    UnloadRkInfoBar();
}

// this is called every frame, keep that in mind.
void RkHudInfoBar::ShowPanel(bool bShow, bool force)
{
	if( !m_pInstance || !m_dataModel )
        return;

    if( bShow )
    {
        if( !m_bVisible )
        {
            m_pInstance->Show();
        }

        float curtime = gHUD.m_flTime;
        if (curtime <= 0.0f)
        {
            curtime = (float)RocketSystem::m_Instance.GetElapsedTime();
        }

        float dt = gHUD.m_flTimeDelta;
        if (dt <= 0.0f || dt > 0.1f)
        {
            if (m_flLastTime > 0.0f && curtime > m_flLastTime)
            {
                dt = curtime - m_flLastTime;
            }
            else
            {
                dt = 0.016f;
            }
        }
        if (dt > 0.1f) dt = 0.1f;
        if (dt < 0.0f) dt = 0.0f;
        m_flLastTime = curtime;

        Rml::Dictionary params;
        params["dt"] = dt;
        params["curtime"] = curtime;
        params["hp"] = infoBarData.hp;
        m_pInstance->DispatchEvent("hud_update", params);

        m_dataModel.DirtyVariable( "hp" );
        m_dataModel.DirtyVariable( "ammo" );
        m_dataModel.DirtyVariable( "ammo_reserve" );
        m_dataModel.DirtyVariable( "fire_mode_string" );
        m_dataModel.DirtyVariable( "armor" );
        m_dataModel.DirtyVariable( "has_helmet" );
        m_dataModel.DirtyVariable( "primary_string" );
        m_dataModel.DirtyVariable( "secondary_string" );
        m_dataModel.DirtyVariable( "knife_string" );
		m_dataModel.DirtyVariable("has_ammo_reserve");
		m_dataModel.DirtyVariable("col_r");
		m_dataModel.DirtyVariable("col_g");
		m_dataModel.DirtyVariable("col_b");
		m_dataModel.DirtyVariable("num_kills");
		m_dataModel.DirtyVariable("ammo_secondary");
	}
	else
	{
		if (m_bVisible)
		{
			m_pInstance->Hide();
		}
	}

    m_bVisible = bShow;
}

void RkHudInfoBar::SetActive(bool bActive)
{
    ShowPanel( bActive, false );
}

bool RkHudInfoBar::ShouldDraw()
{
    return 1;
}

void RkHudInfoBar::UpdateHealth(int new_hp)
{
    if (infoBarData.hp != new_hp)
    {
        int old_hp = infoBarData.hp;
        infoBarData.hp = new_hp;

        if (m_dataModel)
        {
            m_dataModel.DirtyVariable("hp");
        }

        if (m_pInstance)
        {
            float curtime = gHUD.m_flTime;
            if (curtime <= 0.0f)
            {
                curtime = (float)RocketSystem::m_Instance.GetElapsedTime();
            }

            Rml::Dictionary params;
            params["old_hp"] = old_hp;
            params["new_hp"] = new_hp;
            params["curtime"] = curtime;
            m_pInstance->DispatchEvent("hp_change", params);
        }
    }
}

CON_COMMAND( rocket_test_damage, "Simulate taking damage for testing health bar animation (usage: rocket_test_damage [amount])" )
{
    int dmg = 25;
    if (ConCommand::ArgC() > 1)
    {
        dmg = atoi(ConCommand::ArgV(1));
        if (dmg <= 0) dmg = 25;
    }

    int curHp = RkHudInfoBar::infoBarData.hp;
    int newHp = curHp - dmg;
    if (newHp <= 0)
    {
        Msg("[RocketUI] HP depleted (%d -> 0), resetting to 100 for testing.\n", curHp);
        RkHudInfoBar::m_Instance.UpdateHealth(100);
    }
    else
    {
        Msg("[RocketUI] Simulating damage: %d -> %d (-%d HP)\n", curHp, newHp, dmg);
        RkHudInfoBar::m_Instance.UpdateHealth(newHp);
    }
}
