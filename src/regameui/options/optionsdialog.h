//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================
#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI2.h>

namespace vgui2
{
class PropertySheet;
}

#include <vgui_controls/PropertyDialog.h>

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class COptionsDialog : public vgui2::PropertyDialog
{
	DECLARE_CLASS_SIMPLE(COptionsDialog, vgui2::PropertyDialog);

public:
    COptionsDialog(vgui2::Panel *parent);
    ~COptionsDialog();

    void Run();
	virtual void Activate(int tab = 0);

protected:
    virtual void SetTitle(const char *title, bool surfaceTitle);
    virtual void OnClose();
};

#endif // OPTIONSDIALOG_H
