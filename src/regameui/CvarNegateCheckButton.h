//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef CVARNEGATECHECKBUTTON_H
#define CVARNEGATECHECKBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/CheckButton.h>

class CCvarNegateCheckButton : public vgui2::CheckButton
{
    DECLARE_CLASS_SIMPLE( CCvarNegateCheckButton, vgui2::CheckButton );

public:
    CCvarNegateCheckButton( vgui2::Panel *parent, const char *panelName, const char *text,
        char const *cvarname );
    ~CCvarNegateCheckButton();

    virtual void	SetSelected( bool state );
    virtual void	Paint();

    void			Reset();
    void			ApplyChanges();
    bool			HasBeenModified();

private:
    MESSAGE_FUNC( OnButtonChecked, "CheckButtonChecked" );

    char			*m_pszCvarName;
    bool			m_bStartState;
};

#endif // CVARNEGATECHECKBUTTON_H
