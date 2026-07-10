//
// Created by den4ik on 04.01.26.
//

#ifndef BUGFIXEDHL_GAMEMENU_H
#define BUGFIXEDHL_GAMEMENU_H

#include "vgui_controls/Panel.h"
#include "vgui_controls/PHandle.h"
#include "vgui_controls/MenuItem.h"
#include "vgui_controls/MessageDialog.h"
#include "KeyValues.h"
#include "utlvector.h"
#include "EngineInterface.h"
#include "IEngineVGui.h"
#include "GameUI_Interface.h"

#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IVGui.h>
#include "GameMenu.h"
#include "UtlSortVector.h"
//#include "tier1/CommandBuffer.h"
using namespace vgui2;
class CGameMenuItem : public vgui2::MenuItem
{
	DECLARE_CLASS_SIMPLE( CGameMenuItem, vgui2::MenuItem );
public:
	CGameMenuItem(vgui2::Menu *parent, const char *name);

	virtual void ApplySchemeSettings( vgui2::IScheme *pScheme );
	virtual void PaintBackground( void );
	void SetRightAlignedText( bool state );

private:
	bool		m_bRightAligned;
};
class CGameMenu : public vgui2::Menu
{
public:
	DECLARE_CLASS_SIMPLE( CGameMenu, vgui2::Menu );

	CGameMenu(vgui2::Panel *parent, const char *name) : BaseClass(parent, name)
	{
		m_hMainMenuOverridePanel = NULL;
	};
	virtual ~CGameMenu();
	virtual void ApplySchemeSettings(IScheme *pScheme)
	{
		BaseClass::ApplySchemeSettings(pScheme);

		int height = atoi(pScheme->GetResourceString("MainMenu.MenuItemHeight"));
		if( IsProportional() )
			height = scheme()->GetProportionalScaledValue( height );

		// make fully transparent
		SetMenuItemHeight(height);
		SetBgColor(Color(0, 0, 0, 0));
		SetBorder(NULL);
	}

	virtual void LayoutMenuBorder()
	{
	}

	void SetMainMenuOverride( vgui2::VPANEL panel )
	{
		m_hMainMenuOverridePanel = panel;

		if ( m_hMainMenuOverridePanel )
		{
			// We've got an override panel. Nuke all our menu items.
			DeleteAllItems();
		}
	}
	virtual void SetVisible(bool state)
	{
		if ( m_hMainMenuOverridePanel )
		{
			// force to be always visible
			ipanel()->SetVisible( m_hMainMenuOverridePanel, true );

			// move us to the back instead of going invisible
			if ( !state )
			{
				ipanel()->MoveToBack(m_hMainMenuOverridePanel);
			}
		}

		// force to be always visible
		BaseClass::SetVisible(true);

		// move us to the back instead of going invisible
		if (!state)
		{
			ipanel()->MoveToBack(GetVPanel());
		}
	}

	virtual int AddMenuItem(const char *itemName, const char *itemText, const char *command, Panel *target, KeyValues *userData = NULL)
	{
		MenuItem *item = new CGameMenuItem(this, itemName);
		item->AddActionSignalTarget(target);
		item->SetCommand(command);
		item->SetText(itemText);
		item->SetUserData(userData);
		return BaseClass::AddMenuItem(item);
	}

	virtual int AddMenuItem(const char *itemName, wchar_t *itemText, const char *command, Panel *target, KeyValues *userData = NULL)
	{
		MenuItem *item = new CGameMenuItem(this, itemName);
		item->AddActionSignalTarget(target);
		item->SetCommand(command);
		item->SetText(itemText);
		item->SetUserData(userData);
		return BaseClass::AddMenuItem(item);
	}

	virtual int AddMenuItem(const char *itemName, const char *itemText, KeyValues *command, Panel *target, KeyValues *userData = NULL)
	{
		CGameMenuItem *item = new CGameMenuItem(this, itemName);
		item->AddActionSignalTarget(target);
		item->SetCommand(command);
		item->SetText(itemText);
		item->SetRightAlignedText(true);
		item->SetUserData(userData);
		return BaseClass::AddMenuItem(item);
	}

	virtual void SetMenuItemBlinkingState( const char *itemName, bool state )
	{
		for (int i = 0; i < GetChildCount(); i++)
		{
			Panel *child = GetChild(i);
			MenuItem *menuItem = dynamic_cast<MenuItem *>(child);
			if (menuItem)
			{
				if ( Q_strcmp( menuItem->GetCommand()->GetString("command", ""), itemName ) == 0 )
				{
					menuItem->SetBlink( state );
				}
			}
		}
		InvalidateLayout();
	}

	virtual void OnSetFocus()
	{
		if ( m_hMainMenuOverridePanel )
		{
			Panel *pMainMenu = ipanel()->GetPanel( m_hMainMenuOverridePanel, "ClientDLL" );
			if ( pMainMenu )
			{
				pMainMenu->PerformLayout();
			}
		}

		BaseClass::OnSetFocus();
	}

	virtual void OnCommand(const char *command)
	{
		//		m_KeyRepeat.Reset();


		if (!stricmp(command, "Open"))
		{
			if ( m_hMainMenuOverridePanel )
			{
				// force to be always visible
				ipanel()->MoveToFront( m_hMainMenuOverridePanel );
				ipanel()->RequestFocus( m_hMainMenuOverridePanel );
			}
			else
			{
				MoveToFront();
				RequestFocus();
			}
		}
		else
		{
			BaseClass::OnCommand(command);
		}
	}

	virtual void OnKeyCodePressed( KeyCode code )
	{
		if ( IsX360() )
		{
			if ( GetAlpha() != 255 )
			{
				SetEnabled( false );
				// inhibit key activity during transitions
				return;
			}

			SetEnabled( true );

			if ( code == KEY_XBUTTON_B || code == KEY_XBUTTON_START )
			{
				if ( GameUI().IsInLevel() )
				{
					GetParent()->OnCommand( "ResumeGame" );
				}
				return;
			}
		}

		//		m_KeyRepeat.KeyDown( code );

		int nDir = 0;

		switch ( code )
		{
		case KEY_XBUTTON_UP:
		case KEY_XSTICK1_UP:
		case KEY_XSTICK2_UP:
		case KEY_UP:
		case KEY_XBUTTON_DOWN:
		case KEY_XSTICK1_DOWN:
		case KEY_XSTICK2_DOWN:
		case KEY_DOWN:
		default:
			break;
		}
		if ( nDir != 0 )
		{
			CUtlSortVector< SortedPanel_t, CSortedPanelYLess > vecSortedButtons;
			VguiPanelGetSortedChildButtonList( this, (void*)&vecSortedButtons );

			if ( VguiPanelNavigateSortedChildButtonList( (void*)&vecSortedButtons, nDir ) != -1 )
			{
				// Handled!
				return;
			}
		}

		BaseClass::OnKeyCodePressed( code );

		// HACK: Allow F key bindings to operate even here
		if ( IsPC() && code >= KEY_F1 && code <= KEY_F12 )
		{
			// See if there is a binding for the FKey
			//			const char *binding = gameuifuncs->GetBindingForButtonCode( code );
			//			if ( binding && binding[0] )
			//			{
			//				// submit the entry as a console commmand
			//				char szCommand[256];
			//				Q_strncpy( szCommand, binding, sizeof( szCommand ) );
			//				gEngfuncs.pfnClientCmd( szCommand );
			//			}
		}
	}

	void OnKeyCodeReleased( vgui2::KeyCode code )
	{
		//		m_KeyRepeat.KeyUp( code );

		BaseClass::OnKeyCodeReleased( code );
	}

	void OnThink()
	{
		//		vgui2::KeyCode code = m_KeyRepeat.KeyRepeated();
		//		if ( code )
		//		{
		//			OnKeyCodeTyped( code );
		//		}

		BaseClass::OnThink();
	}

	virtual void OnKillFocus()
	{
		BaseClass::OnKillFocus();

		if ( m_hMainMenuOverridePanel )
		{
			// force us to the rear when we lose focus (so it looks like the menu is always on the background)
			surface()->MovePopupToBack( m_hMainMenuOverridePanel );
		}
		else
		{
			// force us to the rear when we lose focus (so it looks like the menu is always on the background)
			surface()->MovePopupToBack(GetVPanel());
		}

		//		m_KeyRepeat.Reset();
	}

	void ShowFooter( bool bShow )
	{
	}

	void UpdateMenuItemState( bool isInGame, bool isMultiplayer, bool isInReplay, bool isVREnabled, bool isVRActive )
	{
		//		bool isSteam = IsPC() && ( GetCommandLine()->FindParm("-steam") != 0 );

		// disabled save button if we're not in a game
		for (int i = 0; i < GetChildCount(); i++)
		{
			Panel *child = GetChild(i);
			MenuItem *menuItem = dynamic_cast<MenuItem *>(child);
			if (menuItem)
			{
				bool shouldBeVisible = true;
				// filter the visibility
				KeyValues *kv = menuItem->GetUserData();
				if (!kv)
					continue;

				if (!isInGame && kv->GetInt("OnlyInGame") )
				{
					shouldBeVisible = false;
				}
				if (!isInReplay && kv->GetInt("OnlyInReplay") )
				{
					shouldBeVisible = false;
				}
				else if (!isVREnabled && kv->GetInt("OnlyWhenVREnabled") )
				{
					shouldBeVisible = false;
				}
				else if (isMultiplayer && kv->GetInt("notmulti"))
				{
					shouldBeVisible = false;
				}
				else if (isInGame && !isMultiplayer && kv->GetInt("notsingle"))
				{
					shouldBeVisible = false;
				}
				//				else if (isSteam && kv->GetInt("notsteam"))
				//				{
				//					shouldBeVisible = false;
				//				}
				//				else if ( !bIsConsoleUI && kv->GetInt( "ConsoleOnly" ) )
				//				{
				//					shouldBeVisible = false;
				//				}

				// If we're playing back a replay, hide everything else
				if ( isInReplay && !kv->GetInt("OnlyInReplay") )
				{
					shouldBeVisible = false;
				}

				menuItem->SetVisible( shouldBeVisible );
			}
		}

		if ( !isInGame )
		{
			// Sort them into their original order
			for ( int j = 0; j < GetChildCount() - 2; j++ )
			{
				MoveMenuItem( j, j + 1 );
			}
		}
		else
		{
			// Sort them into their in game order
			for ( int i = 0; i < GetChildCount(); i++ )
			{
				for ( int j = i; j < GetChildCount() - 2; j++ )
				{
					int iID1 = GetMenuID( j );
					int iID2 = GetMenuID( j + 1 );

					MenuItem *menuItem1 = GetMenuItem( iID1 );
					MenuItem *menuItem2 = GetMenuItem( iID2 );

					KeyValues *kv1 = menuItem1->GetUserData();
					KeyValues *kv2 = menuItem2->GetUserData();

					if( !kv1 || !kv2 )
						continue;

					if ( kv1->GetInt("InGameOrder") > kv2->GetInt("InGameOrder") )
						MoveMenuItem( iID2, iID1 );
				}
			}
		}

		InvalidateLayout();
	}

//	MESSAGE_FUNC_HANDLE( OnCursorEnteredMenuItem, "CursorEnteredMenuItem", menuItem);

private:
	//	CFooterPanel *m_pConsoleFooter;
	//	vgui2::CKeyRepeatHandler	m_KeyRepeat;
	vgui2::VPANEL	m_hMainMenuOverridePanel;
};


#endif //BUGFIXEDHL_GAMEMENU_H
