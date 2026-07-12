#include "rkhud_infopanel.h"

// #include "cbase.h"
// #include "hud_macros.h"
// #include "c_cs_player.h"

// #include <tier0/valve_minmax_off.h> // included to fix an error with min/max and rocketui
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#pragma pop_macro("Assert")
// DECLARE_CLASS_SIMPLE( RkHudInfoBar );

// Struct layout for data-binding model.

documentReloadFuncs docReloadFuncs;

RkHudInfoBar RkHudInfoBar::m_Instance = RkHudInfoBar("hud_infopanel");
RkHudInfoBar::InfoBarData RkHudInfoBar::infoBarData = {
	0, 0, 0, 0, 0,
	"0", "0", "0", "0", 0, 255, 255, 255, 0
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
{
    // SetHiddenBits( /* HIDEHUD_MISCSTATUS */ 0 );
    m_Instance = *this;
}

RkHudInfoBar::~RkHudInfoBar() noexcept
{
    // void UnloadRkInfoBar();
    // UnloadRkInfoBar();
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
    void LoadRkInfoBar();
    LoadRkInfoBar();
}

void RkHudInfoBar::LevelShutdown()
{
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
        // UpdateInfoFromPlayer( *pPlayer );

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

		// m_dataModel.();
	}
	else
	{
		if (m_bVisible)
		{
			m_pInstance->Hide();
		}
	}

end:
    m_bVisible = bShow;
}

void RkHudInfoBar::SetActive(bool bActive)
{
    ShowPanel( bActive, false );
    // CHudElement::SetActive( bActive );
}

bool RkHudInfoBar::ShouldDraw()
{

    return 1;
}
void RkHudInfoBar::UpdateHealth(int new_hp)
{
    if (infoBarData.hp != new_hp)
    {
        infoBarData.hp = new_hp;

        if (m_dataModel)
        {
            m_dataModel.DirtyVariable("hp");
        }
    }
}
