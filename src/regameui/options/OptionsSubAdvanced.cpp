//========= Copyright � 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#define WIN32_LEAN_AND_MEAN

#include "OptionsSubAdvanced.h"

#include <vgui_controls/Button.h>
#include <vgui_controls/CheckButton.h>
#include <KeyValues.h>
#include <vgui_controls/Label.h>
#include <vgui/ISystem.h>
#include <vgui/ISurface.h>
#include <vgui_controls/RadioButton.h>
#include <vgui_controls/TextEntry.h>
#include "../CvarToggleCheckButton.h"
#include "../EngineInterface.h"
#include "IGameUIFuncs.h"
// #include "ContentControlDialog.h"

using namespace vgui2;
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

/*
class CContentControlCheckButton : public vgui2::CheckButton
{
    typedef vgui2::CheckButton BaseClass;

public:
    CContentControlCheckButton( vgui2::Panel *parent, char const *panelName, char const *text );
    ~CContentControlCheckButton();

    DECLARE_PANELMAP();

    void OnCheckButtonChecked( int state );
private:

    bool	m_bRequirePassword;
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CContentControlCheckButton::CContentControlCheckButton( vgui2::Panel *parent, char const *panelName, char const *text ) :
    vgui2::CheckButton( parent, panelName, text )
{
    AddActionSignalTarget( this );

    m_bRequirePassword = true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CContentControlCheckButton::~CContentControlCheckButton()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CContentControlCheckButton::OnCheckButtonChecked( int state )
{
    if ( m_bRequirePassword )
    {
    }
}

//-----------------------------------------------------------------------------
// Purpose: empty message map
//-----------------------------------------------------------------------------
vgui2::MessageMapItem_t CContentControlCheckButton::m_MessageMap[] =
{
    MAP_MESSAGE_INT( CContentControlCheckButton, "CheckButtonChecked", OnCheckButtonChecked, "state" ),	// custom message
};
IMPLEMENT_PANELMAP( CContentControlCheckButton, BaseClass );
*/

// ********************************************************************************

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
COptionsSubAdvanced::COptionsSubAdvanced(vgui2::Panel *parent) : PropertyPage(parent, NULL)
{
    m_pContentCheckButton = new vgui2::CheckButton( this, "ContentlockButton", "#GameUI_ContentLock" );
    m_pContentCheckButton->SetCommand( "ContentControl" );
    //	vgui2::Label *l2 = new vgui2::Label( this, "ContentlockLabel", "#GameUI_ContentLockLabel" );
	m_pAddonCheckButton = new vgui2::CheckButton(this, "AddonsFolder", "#GameUI_AddonsFolder");
	m_pOriginalModelsCheckButton = new vgui2::CheckButton(this, "OriginalModels", "#GameUI_OriginalModels");
	m_pDetailTexturesCheckButton = new CCvarToggleCheckButton(this, "DetailTextures", "#GameUI_DetailTextures", "r_detailtextures");

	// Read current addons_folder and HD models state from the renderer settings
	m_iOrigAddonsFolder = 0;
	m_iOrigHDModels = 0;
	if ( gameuifuncs )
	{
		char szRenderer[128];
		int iWindowed = 0, iVidLevel = 0;
		gameuifuncs->GetCurrentRenderer( szRenderer, sizeof(szRenderer),
			&iWindowed, &m_iOrigHDModels, &m_iOrigAddonsFolder, &iVidLevel );
	}
	m_pAddonCheckButton->SetSelected( m_iOrigAddonsFolder != 0 );
	// "Original models" = HD models OFF
	m_pOriginalModelsCheckButton->SetSelected( m_iOrigHDModels == 0 );

	LoadControlSettings("Resource\\OptionsSubAdvanced.res");

	// set up the content control dialog
    // m_pContentControlDialog = new CContentControlDialog(this);
    // m_pContentControlDialog->AddActionSignalTarget(this);

    int x, y, ww, wt, wide, tall;
    surface()->GetWorkspaceBounds( x, y, ww, wt );
    // m_pContentControlDialog->GetSize(wide, tall);

    // Center it, keeping requested size
    // m_pContentControlDialog->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));

    // m_pContentCheckButton->SetSelected( m_pContentControlDialog->IsPasswordEnabled() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COptionsSubAdvanced::OnCommand( const char *command )
{
    // if ( !stricmp( command, "ContentControl" ) )
    // {
    //     // we're trying to turn it off
    //     if (!m_pContentCheckButton->IsSelected())
    //     {
    //         // if there's no password originally, we can just reset it and move on
    //         if (!m_pContentControlDialog->IsPasswordEnabled())
    //         {
    //             m_pContentControlDialog->ResetPassword();
    //             return;
    //         }
    //     }
    //     OnOpenContentControlDialog();
    //     return;
    // }

    BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
COptionsSubAdvanced::~COptionsSubAdvanced()
{

    //    m_pContentControlDialog->MarkForDeletion();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COptionsSubAdvanced::OnResetData()
{
    // m_pContentControlDialog->ResetPassword();
    // m_pContentCheckButton->SetSelected( m_pContentControlDialog->IsPasswordEnabled() );

	// Re-read addons_folder and HD models from the renderer
	m_iOrigAddonsFolder = 0;
	m_iOrigHDModels = 0;
	if ( gameuifuncs )
	{
		char szRenderer[128];
		int iWindowed = 0, iVidLevel = 0;
		gameuifuncs->GetCurrentRenderer( szRenderer, sizeof(szRenderer),
			&iWindowed, &m_iOrigHDModels, &m_iOrigAddonsFolder, &iVidLevel );
	}
	m_pAddonCheckButton->SetSelected( m_iOrigAddonsFolder != 0 );
	m_pOriginalModelsCheckButton->SetSelected( m_iOrigHDModels == 0 );

	m_pDetailTexturesCheckButton->Reset();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COptionsSubAdvanced::OnApplyChanges()
{
    // m_pContentControlDialog->ApplyPassword();

	// Apply detail textures (simple cvar)
	m_pDetailTexturesCheckButton->ApplyChanges();

	if ( !gameuifuncs )
		return;

	int iNewAddons = m_pAddonCheckButton->IsSelected() ? 1 : 0;
	// "Original models" checked = HD models OFF
	int iNewHDModels = m_pOriginalModelsCheckButton->IsSelected() ? 0 : 1;

	// Only call _setrenderer if either changed
	if ( iNewAddons == m_iOrigAddonsFolder && iNewHDModels == m_iOrigHDModels )
		return;

	// Read current renderer settings so we don't overwrite windowed/renderer
	char szRenderer[128];
	int iWindowed = 0, iHDModels = 0, iAddons = 0, iVidLevel = 0;
	gameuifuncs->GetCurrentRenderer( szRenderer, sizeof(szRenderer),
		&iWindowed, &iHDModels, &iAddons, &iVidLevel );

	char szCmd[256];
	Q_snprintf( szCmd, sizeof(szCmd), "_setrenderer %s %s %s %s\n",
		szRenderer,
		iWindowed ? "windowed" : "fullscreen",
		iNewHDModels ? "hdmodels" : "nohdmodels",
		iNewAddons ? "addons" : "noaddons" );
	gEngfuncs.pfnClientCmd( szCmd );

	// Engine needs a restart to apply filesystem changes
	gEngfuncs.pfnClientCmd( "_restart\n" );

	m_iOrigAddonsFolder = iNewAddons;
	m_iOrigHDModels = iNewHDModels;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COptionsSubAdvanced::OnContentControlClose()
{
    // m_pContentCheckButton->SetSelected(m_pContentControlDialog->IsPasswordEnabledInDialog());

    // if (m_pContentControlDialog->IsPasswordEnabled() != m_pContentControlDialog->IsPasswordEnabledInDialog())
    // {
    //     PostActionSignal(new KeyValues("ApplyButtonEnable"));
    // }
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void COptionsSubAdvanced::OnOpenContentControlDialog()
{
    // m_pContentControlDialog->Activate();
}

//-----------------------------------------------------------------------------
// Purpose: empty message map
//-----------------------------------------------------------------------------
vgui2::MessageMapItem_t COptionsSubAdvanced::m_MessageMap[] =
    {
        //	MAP_MESSAGE( COptionsSubAdvanced, "ControlModified", OnControlModified ),	// custom message
        MAP_MESSAGE( COptionsSubAdvanced, "ContentControlClose", OnContentControlClose ),
    };
IMPLEMENT_PANELMAP( COptionsSubAdvanced, BaseClass );
