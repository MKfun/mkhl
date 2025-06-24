//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#if !defined( VCONTROLSLISTPANEL_H )
#define VCONTROLSLISTPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/SectionedListPanel.h>

//-----------------------------------------------------------------------------
// Purpose: Special list subclass to handle drawing of trap mode prompt on top of
//			lists client area
//-----------------------------------------------------------------------------
class VControlsListPanel : public vgui2::SectionedListPanel
{
public:
	// Construction
					VControlsListPanel( vgui2::Panel *parent, const char *listName );
	virtual			~VControlsListPanel();

	// Start/end capturing
	virtual void	StartCaptureMode(vgui2::HCursor hCursor = NULL);
	virtual void	EndCaptureMode(vgui2::HCursor hCursor = NULL);
	virtual bool	IsCapturing();

	// Set which item should be associated with the prompt
	virtual void	SetItemOfInterest(int itemID);
	virtual int		GetItemOfInterest();

	virtual void	OnMousePressed(vgui2::MouseCode code);
	virtual void	OnMouseDoublePressed(vgui2::MouseCode code);

private:
	void ApplySchemeSettings(vgui2::IScheme *pScheme );

	// Are we showing the prompt?
	bool			m_bCaptureMode;
	// If so, where?
	int				m_nClickRow;
	// Font to use for showing the prompt
	vgui2::HFont		m_hFont;
	// panel used to edit
	class CInlineEditPanel *m_pInlineEditPanel;
	int m_iMouseX, m_iMouseY;

	typedef vgui2::SectionedListPanel BaseClass;
};

#endif // VCONTROLSLISTPANEL_H
