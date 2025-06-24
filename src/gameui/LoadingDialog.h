//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef LOADINGDIALOG_H
#define LOADINGDIALOG_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>

//-----------------------------------------------------------------------------
// Purpose: Dialog for displaying level loading status
//-----------------------------------------------------------------------------
class CLoadingDialog : public vgui2::Frame
{
public:
	CLoadingDialog(vgui2::Panel *parent);
	~CLoadingDialog();

	void DisplayProgressBar(const char *resourceType, const char *resourceName);
	void DisplayError(const char *failureReason, const char *extendedReason = NULL);
	void SetProgressPoint(int progressPoint);
	void SetProgressRange(int min, int max);
	void SetStatusText(const char *statusText);
	void SetSecondaryProgress(float progress);
	void SetSecondaryProgressText(const char *statusText);

protected:
	virtual void OnCommand(const char *command);
	virtual void PerformLayout();
	virtual void OnThink();
	virtual void OnClose();
	virtual void OnKeyCodePressed(vgui2::KeyCode code);

private:
	vgui2::ProgressBar *m_pProgress;
	vgui2::ProgressBar *m_pProgress2;
	vgui2::Label *m_pInfoLabel;
	vgui2::Label *m_pTimeRemainingLabel;
	vgui2::Button *m_pCancelButton;
	vgui2::HTML *m_pHTML;

	int m_iRangeMin, m_iRangeMax;
	bool m_bShowingSecondaryProgress;
	float m_flSecondaryProgress;
	float m_flLastSecondaryProgressUpdateTime;
	float m_flSecondaryProgressStartTime;

	typedef vgui2::Frame BaseClass;
};

// singleton accessor
CLoadingDialog *LoadingDialog();


#endif // LOADINGDIALOG_H
