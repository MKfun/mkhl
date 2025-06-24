//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#ifndef PANELLISTPANEL_H
#define PANELLISTPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/Panel.h>

class KeyValues;

namespace vgui { class ScrollBar; }

//-----------------------------------------------------------------------------
// Purpose: A list of variable height child panels
//-----------------------------------------------------------------------------
class CPanelListPanel : public vgui2::Panel
{
	typedef vgui2::Panel BaseClass;

public:
	typedef struct dataitem_s
	{
		// Always store a panel pointer
		vgui2::Panel *panel;
	} DATAITEM;

	CPanelListPanel( vgui2::Panel *parent, char const *panelName );
	~CPanelListPanel();

	// DATA & ROW HANDLING
	// The list now owns the panel
	virtual int	computeVPixelsNeeded( void );
	virtual int AddItem( vgui2::Panel *panel );
	virtual int	GetItemCount( void );
	virtual vgui2::Panel *GetItem(int itemIndex); // returns pointer to data the row holds
	virtual void RemoveItem(int itemIndex); // removes an item from the table (changing the indices of all following items)
	virtual void DeleteAllItems(); // clears and deletes all the memory used by the data items

	// PAINTING
	virtual vgui2::Panel *GetCellRenderer( int row );

	virtual void OnSliderMoved( int position );

	DECLARE_PANELMAP();
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

protected:

	DATAITEM	*GetDataItem( int itemIndex );

	virtual void PerformLayout();
	virtual void PaintBackground();
	virtual void OnMouseWheeled(int delta);

private:
	// list of the column headers
	vgui2::Dar<DATAITEM *>	_dataItems;
	vgui2::ScrollBar		*_vbar;
	vgui2::Panel			*_embedded;

	int					_tableStartX;
	int					_tableStartY;

};

#endif // PANELLISTPANEL_H
