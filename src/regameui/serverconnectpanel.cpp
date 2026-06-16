#include "serverconnectpanel.h"
// #include "enginecallback.h"
#include "EngineInterface.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/TextEntry.h"
extern cl_enginefunc_t gEngfuncs;
ServerConnectPanel::ServerConnectPanel(vgui2::Panel *parent)
    : vgui2::Frame(parent, "ConnectDialog")
{
	SetDeleteSelfOnClose(true);
	SetTitle("#GameUI_ConnectToServer", true);
	m_pConnectBtn = new vgui2::Button(this, "ConnectBtn", "#GameUI_Connect", this, "OnConnect");
	m_pCancelBtn = new vgui2::Button(this, "CancelBtn", "#GameUI_Cancel", this, "OnCancel");
	m_pServerAddress = new vgui2::TextEntry(this, "SrvAddr");
	m_pServerPassword = new vgui2::TextEntry(this, "SrvPass");
	SetBuildModeEditable(true);
	SetSizeable(false);
	SetMoveable(true);
	SetProportional(false);
	LoadControlSettings("resource/ConnectDialog.res");
}
void ServerConnectPanel::OnCommand(const char *command)
{
	if (!V_strcmp(command, "OnConnect"))
	{
		char buf[255];
		char textbuf[247];
		if (m_pServerPassword->GetTextLength())
		{
			char passbuf[256];
			char bbuf[256];
			m_pServerPassword->GetText(passbuf, 256);
			V_snprintf(bbuf, 256, "password %s", passbuf);
			gEngfuncs.pfnClientCmd(bbuf);
		}
		m_pServerAddress->GetText(textbuf, 247);
		V_sprintf_safe(buf, "connect %s", textbuf);
		gEngfuncs.pfnClientCmd(buf);
	}
	else if (!V_strcmp(command, "OnCancel"))
	{
		Close();
	}
	vgui2::Frame::OnCommand(command);
}
