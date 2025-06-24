//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef CONTENTCONTROLDIALOG_H
#define CONTENTCONTROLDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include "taskframe.h"

namespace vgui
{
	class Label;
	class Button;
	class TextEntry;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CContentControlDialog : public CTaskFrame
{
public:
	CContentControlDialog(vgui2::Panel *parent);
	~CContentControlDialog();

	virtual void OnCommand( const char *command );
	virtual void OnClose();
	virtual void Activate();

    void        ResetPassword();
    void        ApplyPassword();
    bool        IsPasswordEnabledInDialog();
    bool        IsPasswordEnabled()             { return ( m_szGorePW[0] != 0 ); }

protected:
	void			WriteToken( const char *str );
	bool			CheckPassword( char const *oldPW, char const *newPW, bool enableContentControl );
	char			*BinPrintf( unsigned char *buf, int nLen );
	void			UpdateContentControlStatus( void );

	void			Explain( char const *fmt, ... );

    void            HashPassword(const char *newPW, char *hashBuffer);
    bool            EnablePassword(const char *newPW);
    bool            DisablePassword(const char *oldPW);

	enum
	{
		MAX_GORE_PW = 64,
	};

	char			m_szGorePW[ MAX_GORE_PW ];

    bool            m_bDefaultPassword;
	vgui2::Label		*m_pStatus;
	vgui2::Button	*m_pOK;
	vgui2::TextEntry	*m_pPassword;
    vgui2::Label     *m_pPasswordLabel;
    vgui2::Label     *m_pPassword2Label;
	vgui2::TextEntry	*m_pPassword2;

	vgui2::Label		*m_pExplain;

	typedef CTaskFrame BaseClass;
};


#endif // CONTENTCONTROLDIALOG_H
