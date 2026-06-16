#ifndef SERVERCONNECTPANEL_H
#define SERVERCONNECTPANEL_H

#include "vgui_controls/EditablePanel.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/Panel.h"
class ServerConnectPanel : public vgui2::Frame
{
public:
	ServerConnectPanel(vgui2::Panel *parent);
	virtual void OnCommand(const char *command) override;

private:
	vgui2::Button *m_pConnectBtn;
	vgui2::Button *m_pCancelBtn;
	vgui2::TextEntry *m_pServerAddress;
	vgui2::TextEntry *m_pServerPassword;
};

#endif // SERVERCONNECTPANEL_H
