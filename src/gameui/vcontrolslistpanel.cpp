//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "vcontrolslistpanel.h"
#include "GameUI_Interface.h"
#include "EngineInterface.h"

#include <vgui/IInput.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/Cursor.h>
#include <KeyValues.h>


// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui2;

//-----------------------------------------------------------------------------
// Purpose: panel used for inline editing of key bindings
//-----------------------------------------------------------------------------
class CInlineEditPanel : public vgui2::Panel
{
public:
	CInlineEditPanel() : vgui2::Panel(NULL, "InlineEditPanel")
	{
	}

	virtual void Paint()
	{
		int x = 0, y = 0, wide, tall;
		GetSize(wide, tall);

		// Draw a white rectangle around that cell
		vgui2::surface()->DrawSetColor( 149, 136, 49, 255 );
		vgui2::surface()->DrawFilledRect( x, y, x + wide, y + tall );

		// top & left
		vgui2::surface()->DrawSetColor( 45, 49, 40, 255 );
		vgui2::surface()->DrawFilledRect( x - 3, y - 3, x + wide + 2, y - 2 );
		vgui2::surface()->DrawFilledRect( x - 2, y - 2, x - 3, y + tall + 3 );

		// bottom and right
		vgui2::surface()->DrawSetColor( 150, 159, 142, 255 );
		vgui2::surface()->DrawFilledRect( x - 2, y + tall + 3, x + wide + 3, y + tall + 2 );
		vgui2::surface()->DrawFilledRect( x + wide + 3, y - 3, x + wide + 2, y + tall + 2 );
	}

	virtual void OnKeyCodeTyped(KeyCode code)
	{
		// forward up
		if (GetParent())
		{
			GetParent()->OnKeyCodeTyped(code);
		}
	}

	virtual void ApplySchemeSettings(IScheme *pScheme)
	{
		Panel::ApplySchemeSettings(pScheme);
		SetBorder(pScheme->GetBorder("DepressedButtonBorder"));
	}

	void OnMousePressed(vgui2::MouseCode code)
	{
		// forward up mouse pressed messages to be handled by the key options
		if (GetParent())
		{
			GetParent()->OnMousePressed(code);
		}
	}
};

//-----------------------------------------------------------------------------
// Purpose: Construction
//-----------------------------------------------------------------------------
VControlsListPanel::VControlsListPanel( vgui2::Panel *parent, const char *listName )	: vgui2::SectionedListPanel( parent, listName )
{
	m_bCaptureMode	= false;
	m_nClickRow		= 0;
	m_pInlineEditPanel = new CInlineEditPanel();
	m_hFont = INVALID_FONT;
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
VControlsListPanel::~VControlsListPanel()
{
	m_pInlineEditPanel->MarkForDeletion();
}

void VControlsListPanel::ApplySchemeSettings(IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
	m_hFont	= pScheme->GetFont("Default", IsProportional() ); 
}

//-----------------------------------------------------------------------------
// Purpose: Start capture prompt display
//-----------------------------------------------------------------------------
void VControlsListPanel::StartCaptureMode( HCursor hCursor )
{
	m_bCaptureMode = true;
	EnterEditMode(m_nClickRow, 1, m_pInlineEditPanel);
	input()->SetMouseFocus(m_pInlineEditPanel->GetVPanel());
	input()->SetMouseCapture(m_pInlineEditPanel->GetVPanel());

	if (hCursor)
	{
		m_pInlineEditPanel->SetCursor(hCursor);
		if ( hCursor == dc_none )
		{
			vgui2::input()->GetCursorPos( m_iMouseX, m_iMouseY );
			engine->SetCursorPos(  engine->GetWindowCenterX(), engine->GetWindowCenterY() ); // use gEngfuncs and NOT input() because input() is relative to vgui, not absolute
		}
	}
	// turn off key translation (so that we can tell the difference between ENTER and KP_ENTER)
	vgui2::surface()->SetTranslateExtendedKeys(false);
}

//-----------------------------------------------------------------------------
// Purpose: Finish capture prompt display
//-----------------------------------------------------------------------------
void VControlsListPanel::EndCaptureMode( HCursor hCursor )
{
	m_bCaptureMode = false;
	input()->SetMouseCapture(NULL);
	// reset key translation (so that KP_ENTER will act like a normal enter key again)
	vgui2::surface()->SetTranslateExtendedKeys(true);
	LeaveEditMode();
	RequestFocus();
	input()->SetMouseFocus(GetVPanel());
	if (hCursor)
	{
		m_pInlineEditPanel->SetCursor(hCursor);
		if ( GameUI().IsGameUIActive() )
		{
			surface()->SetCursor(hCursor);	
			if ( hCursor != dc_none )
			{
				vgui2::input()->SetCursorPos ( m_iMouseX, m_iMouseY );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Set active row column
//-----------------------------------------------------------------------------
void VControlsListPanel::SetItemOfInterest(int itemID)
{
	m_nClickRow	= itemID;
}

//-----------------------------------------------------------------------------
// Purpose: Retrieve row, column of interest
//-----------------------------------------------------------------------------
int VControlsListPanel::GetItemOfInterest()
{
	return m_nClickRow;
}

//-----------------------------------------------------------------------------
// Purpose: returns true if we're currently waiting to capture a key
//-----------------------------------------------------------------------------
bool VControlsListPanel::IsCapturing( void )
{
	return m_bCaptureMode;
}

//-----------------------------------------------------------------------------
// Purpose: Forwards mouse pressed message up to keyboard page when in capture
//-----------------------------------------------------------------------------
void VControlsListPanel::OnMousePressed(vgui2::MouseCode code)
{
	if (IsCapturing())
	{
		// forward up mouse pressed messages to be handled by the key options
		if (GetParent())
		{
			GetParent()->OnMousePressed(code);
		}
	}
	else
	{
		BaseClass::OnMousePressed(code);
	}
}


//-----------------------------------------------------------------------------
// Purpose: input handler
//-----------------------------------------------------------------------------
void VControlsListPanel::OnMouseDoublePressed( vgui2::MouseCode code )
{
	if (IsItemIDValid(GetSelectedItem()))
	{
		// enter capture mode
		OnKeyCodePressed(KEY_ENTER);
	}
	else
	{
		BaseClass::OnMouseDoublePressed(code);
	}
}
