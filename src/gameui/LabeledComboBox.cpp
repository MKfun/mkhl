#include "LabeledComboBox.h"
#include <KeyValues.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CLabeledComboBox::CLabeledComboBox( vgui2::Panel *parent, const char *panelName, const char *label )
 : vgui2::Panel( parent, panelName )
{
	m_pComboBox = new vgui2::ComboBox( this, "LabeledComboBox", 5, false );

	m_pLabel = new vgui2::Label( this, "LabeledComboBoxLabel", label );
	m_pLabel->SetContentAlignment( vgui2::Label::a_west );

	SetSize( 100, 100 );
	SetPaintBackgroundEnabled( false );

	m_pComboBox->AddActionSignalTarget( this );
}

CLabeledComboBox::~CLabeledComboBox( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: default message handler
//-----------------------------------------------------------------------------
void CLabeledComboBox::OnSizeChanged(int newWide, int newTall)
{
	m_pLabel->SetBounds( 2, 2, newWide-4, 15 );
	m_pComboBox->SetBounds( 2, 20, newWide-4, 18 );
}

void CLabeledComboBox::setLabel( char const *text )
{
	m_pLabel->SetText( text );
}

void CLabeledComboBox::RemoveAllItems()
{
	//MODDD - it is called 'DeleteAllItems' ?
	//m_pComboBox->RemoveAllItems();
	m_pComboBox->DeleteAllItems();
}

void CLabeledComboBox::ActivateItem(int itemIndex)
{
	m_pComboBox->ActivateItem( itemIndex );
}

void CLabeledComboBox::AddItem( char const *text )
{
	//MODDD - is sending NULL for the 2nd arg ok?
	m_pComboBox->AddItem( text, NULL);
}

void CLabeledComboBox::OnTextChanged( char const *text )
{
	PostMessage( GetParent()->GetVPanel(), new KeyValues( "TextChanged", "text", text ) );
}

void CLabeledComboBox::GetText( char *buffer, int len )
{
	m_pComboBox->GetText( buffer, len );
}

//-----------------------------------------------------------------------------
// Purpose: Message mapping 
//-----------------------------------------------------------------------------
vgui2::MessageMapItem_t CLabeledComboBox::m_MessageMap[] =
{
	MAP_MESSAGE_CONSTCHARPTR( CLabeledComboBox, "TextChanged", OnTextChanged, "text" ),	// custom message
};

IMPLEMENT_PANELMAP( CLabeledComboBox, BaseClass );
