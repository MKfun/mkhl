//========= Copyright � 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================

#ifndef MULTIPLAYERADVANCEDDIALOG_H
#define MULTIPLAYERADVANCEDDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/PanelListPanel.h>
// #include "TaskFrame.h"
// #include "ScriptObject.h"

// class PanelListPanel;

class CMultiplayerAdvanced
{
protected:
    void CreateControls();
    void DestroyControls();
    void GatherCurrentValues();
    void SaveValues();

    // CInfoDescription *m_pDescription;

    // mpcontrol_t *m_pList;

    vgui2::PanelListPanel *m_pListPanel;
};


class CMultiplayerAdvancedDialog : public vgui2::Frame, public CMultiplayerAdvanced
{
    typedef vgui2::Frame BaseClass;

public:
    CMultiplayerAdvancedDialog(vgui2::Panel *parent);
    ~CMultiplayerAdvancedDialog();

    virtual void OnCommand( const char *command );
    virtual void OnClose();

};


class CMultiplayerAdvancedPage : public vgui2::PropertyPage, public CMultiplayerAdvanced
{
    typedef vgui2::PropertyPage BaseClass;

public:
    CMultiplayerAdvancedPage(vgui2::Panel *parent);
    ~CMultiplayerAdvancedPage();

    virtual void OnResetData();
    virtual void OnApplyChanges();


};

#endif // MULTIPLAYERADVANCEDDIALOG_H
