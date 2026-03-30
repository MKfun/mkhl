#include "rkhud_infopanel.h"

// #include "cbase.h"
// #include "hud_macros.h"
// #include "c_cs_player.h"

// #include <tier0/valve_minmax_off.h> // included to fix an error with min/max and rocketui

#include <RmlUi/Core.h>

// DECLARE_CLASS_SIMPLE( RkHudInfoBar );

// Struct layout for data-binding model.


// static void UpdateInfoFromPlayer( const C_CSPlayer &pPlayer )
// void UpdateInfoFromPlayer() {
//     infoBarData.hp = pPlayer.GetHealth();
//     infoBarData.armor = pPlayer.ArmorValue();
//     infoBarData.hasHelmet = false;
//     if( pPlayer.HasHelmet() )
//         infoBarData.hasHelmet = true;

//     infoBarData.fireModeString = " ";
//     infoBarData.primaryString = " ";
//     infoBarData.secondaryString = " ";
//     infoBarData.knifeString = " ";
//     infoBarData.hasGrenade = false;
//     //infoBarData.hasFlash = false;
//     //infoBarData.hasFlashPair = false;
//     infoBarData.hasSmoke = false;
//     infoBarData.hasFire = false;
//     infoBarData.hasC4 = false;

//     int flashbangAmount = 0;
//     for( int i = 0; i < MAX_WEAPONS; i++ )
//     {
//         CWeaponCSBase *weapon = (CWeaponCSBase*)pPlayer.GetWeapon(i);
//         if( !weapon )
//             continue;

//         int slot = weapon->GetSlot();
//         const char *name;

//         switch( slot )
//         {
//         case WEAPON_SLOT_RIFLE:
//             name = V_strstr(weapon->GetName(), "_");
//             if( name && name[0] )
//                 infoBarData.primaryString = name+1;
//             break;
//         case WEAPON_SLOT_PISTOL:
//             name = V_strstr(weapon->GetName(), "_");
//             if( name && name[0] )
//                 infoBarData.secondaryString = name+1;
//             break;
//         case WEAPON_SLOT_KNIFE:
//             name = V_strstr(weapon->GetName(), "_");
//             if( name && name[0] )
//                 infoBarData.knifeString = name+1;
//             break;
//         case WEAPON_SLOT_GRENADES:
//         {
//             int weaponID = weapon->GetCSWeaponID();
//             switch( weaponID )
//             {
//             case WEAPON_HEGRENADE:
//                 infoBarData.hasGrenade = true;
//                 break;
//             case WEAPON_FLASHBANG:
//                 flashbangAmount++;
//                 break;
//             case WEAPON_SMOKEGRENADE:
//                 infoBarData.hasSmoke = true;
//                 break;
//             case WEAPON_MOLOTOV:
//                 infoBarData.hasFire = true;
//                 break;
//             case WEAPON_INCGRENADE:
//                 infoBarData.hasFire = true;
//                 break;
//             default:
//                 break;
//             }
//             break;
//         }
//         case WEAPON_SLOT_C4:
//             infoBarData.hasC4 = true;
//             break;
//         default:
//             break;
//         }
//     }
//     infoBarData.hasFlash = ( flashbangAmount == 1 );
//     infoBarData.hasFlashPair = ( flashbangAmount == 2 );

//     CWeaponCSBase *activeWeapon = pPlayer.GetActiveCSWeapon();
//     if( activeWeapon )
//     {
//         infoBarData.ammo = activeWeapon->Clip1();
//         infoBarData.ammoReserve = activeWeapon->GetReserveAmmoCount( AMMO_POSITION_PRIMARY );
//         if( activeWeapon->IsFullAuto() )
//             infoBarData.fireModeString = "AUTO";
//         else if( activeWeapon->IsInBurstMode() )
//             infoBarData.fireModeString = "BURST";
//         else
//             infoBarData.fireModeString = "SINGLE";
//     }
// }
RkHudInfoBar RkHudInfoBar::m_Instance = RkHudInfoBar("hud_infopanel");
RkHudInfoBar::InfoBarData RkHudInfoBar::infoBarData =
{
    0,    0,    0,    0,    0,
    "0",    "0",    "0",    "0",    0,    0,    0,    0
};
// void UnloadRkInfoBar()
// {
//     RkHudInfoBar pInfoBar = RkHudInfoBar::m_Instance;
//     // if( !pInfoBar )
//     // {
//     //     return;
//     // }

//     // Not loaded
//     if( !pInfoBar.m_pInstance )
//     {
//         Warning( "Couldn't grab RkHudInfoBar element to unload!\n");
//         return;
//     }

//     Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
//     if( hudCtx )
//     {
//         hudCtx->RemoveDataModel("infobar_model");
//         pInfoBar.m_dataModel = nullptr;
//     }
//     else
//     {
//         Warning("Couldn't access hudCtx to unload infobar datamodel\n");
//     }

//     // pInfoBar.m_pInstance->Close();
//     pInfoBar.m_pInstance = nullptr;
// }

void LoadRkInfoBar()
{
    // 1. Добавлен амперсанд (&), теперь мы работаем с оригиналом!
    RkHudInfoBar &pInfoBar = RkHudInfoBar::m_Instance;

    Rml::Context *hudCtx = RocketUIImpl::m_Instance.AccessHudContext();
    if( !hudCtx )
    {
        Error("Couldn't access hudctx!\n");
        return; // Обязательно return, чтобы не крашнуться ниже
    }

    // 2. Оставляем только эту проверку
    if( pInfoBar.m_pInstance || pInfoBar.m_dataModel )
    {
        Warning("RkInfoBar already loaded, call unload first!\n");
        return;
    }

    // Создаем DataModel
    Rml::DataModelConstructor constructor = hudCtx->CreateDataModel("infobar_model");
    if( !constructor )
    {
        Error( "Couldn't create datamodel for infobar!\n");
        return;
    }

    // Бинды оставляем как были...
    constructor.Bind("hp", &RkHudInfoBar::infoBarData.hp);
    constructor.Bind("armor", &RkHudInfoBar::infoBarData.armor);
    constructor.Bind("ammo", &RkHudInfoBar::infoBarData.ammo);
    constructor.Bind("ammo_reserve", &RkHudInfoBar::infoBarData.ammoReserve);
    constructor.Bind("fire_mode_string", &RkHudInfoBar::infoBarData.fireModeString);
    constructor.Bind("has_helmet", &RkHudInfoBar::infoBarData.hasHelmet);
    constructor.Bind("primary_string", &RkHudInfoBar::infoBarData.primaryString);
    constructor.Bind("secondary_string", &RkHudInfoBar::infoBarData.secondaryString);
    constructor.Bind("knife_string", &RkHudInfoBar::infoBarData.knifeString);
    constructor.Bind("has_grenade", &RkHudInfoBar::infoBarData.hasGrenade);
    constructor.Bind("has_flash", &RkHudInfoBar::infoBarData.hasFlash);
    constructor.Bind("has_flash_pair", &RkHudInfoBar::infoBarData.hasFlashPair);
    constructor.Bind("has_smoke", &RkHudInfoBar::infoBarData.hasSmoke);
    constructor.Bind("has_fire", &RkHudInfoBar::infoBarData.hasFire);
    constructor.Bind("has_c4", &RkHudInfoBar::infoBarData.hasC4);

    pInfoBar.m_dataModel = constructor.GetModelHandle();

    // Загружаем документ
    pInfoBar.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud( "body", "GAME", "rocketui/hud_infobar.rml", 0 );

    if( !pInfoBar.m_pInstance )
    {
        Error("Couldn't create hud_infobar document!\n");
        return;
    }

    // Показываем панель
    pInfoBar.SetActive(1);
}

void UnloadRkInfoBar()
{
    // Добавлен амперсанд (&)
    RkHudInfoBar &pInfoBar = RkHudInfoBar::m_Instance;

    if( !pInfoBar.m_pInstance )
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
    pInfoBar.m_pInstance->Close(); // Не забудьте закрыть документ перед обнулением!
    pInfoBar.m_pInstance = nullptr;
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
    if( !m_pInstance )
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
        m_dataModel.DirtyVariable( "has_grenade" );
        m_dataModel.DirtyVariable( "has_flash" );
        m_dataModel.DirtyVariable( "has_flash_pair" );
        m_dataModel.DirtyVariable( "has_smoke" );
        m_dataModel.DirtyVariable( "has_fire" );
        m_dataModel.DirtyVariable( "has_c4" );

        m_dataModel.Update();
    }
    else
    {
        if( m_bVisible )
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
