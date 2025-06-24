//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#ifndef TASKBUTTON_H
#define TASKBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <Color.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/PHandle.h>
#include <vgui_controls/Image.h>

//-----------------------------------------------------------------------------
// Purpose: Image of the window state
//-----------------------------------------------------------------------------
class CWindowStateImage : public vgui2::Image
{
public:
	CWindowStateImage();

	// sets the height of the image
	void SetTall(int tall);

	// sets the image state
	void SetFocused();
	void SetOutOfFocus();
	void SetMinimized();

private:
	virtual void Paint();
	int m_iState;
};

//-----------------------------------------------------------------------------
// Purpose: Button for a specific window
//-----------------------------------------------------------------------------
class CTaskButton : public vgui2::Label
{
public:
	CTaskButton(vgui2::Panel *parent, vgui2::VPANEL task);
	~CTaskButton();

	// Sets the title of the task button
	void SetTitle(const wchar_t *title);

	// Sets the selection state of the task button
	void SetSelected(bool state);

	// returns true if the task button should be displayed
	bool ShouldDisplay();

	// returns a pointer to the associated task
	vgui2::VPANEL GetTaskPanel();

protected:
	// overrides
	virtual void OnMousePressed(enum vgui2::MouseCode mouseCode);
	virtual void OnMouseDoublePressed(enum vgui2::MouseCode mouseCode);
	virtual void ApplySchemeSettings(vgui2::IScheme *pScheme);

private:
	vgui2::HPanel m_hTask;	// panel handle to the associated task
	bool m_bHasTitle;
	bool m_bSelected;

	Color m_SelectedColor;
	Color m_LabelColor;

	CWindowStateImage m_WindowStateImage;

	typedef vgui2::Label BaseClass;
};


#endif // TASKBUTTON_H
