#include "rkhud_chat.h"

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#pragma pop_macro("Assert")

#include <algorithm>
#include <string>
#include <tier1/strtools.h>
#include "hud.h"
#include "cl_util.h"
#include "hud/chat.h"
#include "gameui/gameui_viewport.h"
#include "sdl_rt.h"
#include "rocketsystem.h"

ConVar rocket_hud_chat_idle_opacity("rocket_hud_chat_idle_opacity", "0.2", 0, "The Opacity of the Chat while it is not active");
ConVar rocket_hud_chat_active_opacity("rocket_hud_chat_active_opacity", "0.7", 0, "The Opacity of the Chat while typing/new message");
ConVar rocket_hud_chat_max_entries("rocket_hud_chat_max_entries", "1000", 0, "Chat History Length");

CON_COMMAND(rocket_hud_chat_clear, "Clears the Chat History")
{
	RkHudChat::m_Instance.ClearChatHistory();
}

class RkHudChatEventListener : public Rml::EventListener
{
public:
	void ProcessEvent(Rml::Event &keyevent) override
	{
		char sayBuffer[1024];
		switch (keyevent.GetId())
		{
		case Rml::EventId::Keydown:
		{
			Rml::Input::KeyIdentifier key_identifier = (Rml::Input::KeyIdentifier)keyevent.GetParameter<int>("key_identifier", 0);
			if (key_identifier == Rml::Input::KI_ESCAPE)
			{
				keyevent.StopPropagation();
				RkHudChat &chat = RkHudChat::m_Instance;
				if (chat.m_elemChatInput)
				{
					Rml::ElementFormControl *input = static_cast<Rml::ElementFormControl *>(chat.m_elemChatInput);
					input->SetValue("");
				}
				chat.StopMessageMode();
			}
			else if (key_identifier == Rml::Input::KI_RETURN || key_identifier == Rml::Input::KI_NUMPADENTER)
			{
				keyevent.StopPropagation();
				RkHudChat &chat = RkHudChat::m_Instance;
				if (!chat.ChatRaised() || !chat.m_elemChatInput)
					break;

				Rml::ElementFormControl *input = static_cast<Rml::ElementFormControl *>(chat.m_elemChatInput);
				Rml::String value = input->GetValue();

				if (!value.empty())
				{
					Q_snprintf(sayBuffer, sizeof(sayBuffer), "%s \"%s\"", chat.GetMessageMode() == MM_SAY ? "say" : "say_team", value.c_str());
					gEngfuncs.pfnClientCmd(sayBuffer);
					input->SetValue("");
				}

				chat.StopMessageMode();
			}
			break;
		}
		default:
			break;
		}
	}
};

static RkHudChatEventListener chatEventListener;

documentReloadFuncs chatDocReloadFuncs;

void UnloadRkChat()
{
	RkHudChat &pChat = RkHudChat::m_Instance;
	if (!pChat.m_pInstance)
		return;

	if (pChat.m_bGrabbingInput)
	{
		RocketUIImpl::m_Instance.DenyInputToGame(false, "Hud_Chat");
		pChat.m_bGrabbingInput = false;
	}

	pChat.m_pInstance->Close();
	pChat.m_pInstance = nullptr;
	pChat.m_elemChatLines = nullptr;
	pChat.m_elemChatInput = nullptr;
	pChat.m_bVisible = false;
	pChat.m_iMode = MM_NONE;
}

void LoadRkChat()
{
	chatDocReloadFuncs.LoadDocument = &LoadRkChat;
	chatDocReloadFuncs.UnloadDocument = &UnloadRkChat;

	RkHudChat &pChat = RkHudChat::m_Instance;

	if (pChat.m_pInstance)
	{
		Warning("RkHudChat already loaded, call unload first!\n");
		return;
	}

	pChat.m_pInstance = RocketUIImpl::m_Instance.LoadDocumentFileIntoHud("body", "GAME", "rocketui/hud_chat.rml", &chatDocReloadFuncs);

	if (!pChat.m_pInstance)
	{
		Error("Couldn't create hud_chat document!\n");
		return;
	}

	pChat.m_elemChatLines = pChat.m_pInstance->GetElementById("chat_lines");
	if (!pChat.m_elemChatLines)
	{
		Warning("Couldn't find required element id: 'chat_lines' in hud_chat\n");
	}

	pChat.m_elemChatInput = pChat.m_pInstance->GetElementById("chat_input");
	if (!pChat.m_elemChatInput)
	{
		Warning("Couldn't find required element id: 'chat_input' in hud_chat\n");
	}

	pChat.m_pInstance->AddEventListener(Rml::EventId::Keydown, &chatEventListener);
	if (pChat.m_elemChatInput)
	{
		pChat.m_elemChatInput->AddEventListener(Rml::EventId::Keydown, &chatEventListener);
	}

	pChat.m_pInstance->Show();
	pChat.m_bVisible = true;
	pChat.m_pInstance->SetClass("chat_open", false);
	pChat.m_pInstance->SetProperty("opacity", "1.0");
}

RkHudChat RkHudChat::m_Instance = RkHudChat("hud_chat");

RkHudChat::RkHudChat(const char *value)
	: m_pInstance(nullptr)
	, m_elemChatLines(nullptr)
	, m_elemChatInput(nullptr)
	, m_bVisible(false)
	, m_bGrabbingInput(false)
	, m_iMode(MM_NONE)
	, m_iNumEntries(0)
{
}

RkHudChat::~RkHudChat() noexcept
{
	UnloadRkChat();
}

void RkHudChat::LevelInit()
{
	LoadRkChat();
}

void RkHudChat::LevelShutdown()
{
	m_iMode = MM_NONE;
	if (m_bGrabbingInput)
	{
		RocketUIImpl::m_Instance.DenyInputToGame(false, "Hud_Chat");
		m_bGrabbingInput = false;
	}
	UnloadRkChat();
}

void RkHudChat::ShowPanel(bool bShow, bool force)
{
	if (!m_pInstance)
		return;

	if (bShow)
	{
		if (!m_bVisible)
		{
			m_pInstance->Show();
			m_bVisible = true;
		}
	}
	else
	{
		if (m_iMode != MM_NONE)
			return;

		if (m_bVisible)
		{
			m_pInstance->Hide();
			m_bVisible = false;
		}
	}
}

void RkHudChat::SetActive(bool bActive)
{
	ShowPanel(bActive, false);
}

bool RkHudChat::ShouldDraw()
{
	return 1;
}

void RkHudChat::Update()
{
	if (!m_pInstance)
		return;

	float curtime = (float)RocketSystem::m_Instance.GetElapsedTime();

	Rml::Dictionary params;
	params["curtime"] = curtime;
	params["is_open"] = ChatRaised();
	m_pInstance->DispatchEvent("chat_update", params);
}

void RkHudChat::StartMessageMode(int mode)
{
	if (ChatRaised())
		return;

	if (!m_pInstance)
		return;

	m_iMode = mode;

	if (!m_bGrabbingInput)
	{
		RocketUIImpl::m_Instance.DenyInputToGame(true, "Hud_Chat");
		m_bGrabbingInput = true;
	}

	if (!m_bVisible)
	{
		m_pInstance->Show();
		m_bVisible = true;
	}

	m_pInstance->SetClass("chat_open", true);
	m_pInstance->DispatchEvent("chat_open", Rml::Dictionary());

	if (m_elemChatInput)
	{
		m_elemChatInput->Focus();
		Rml::ElementFormControl *input = static_cast<Rml::ElementFormControl *>(m_elemChatInput);
		input->SetValue("");
	}

	if (GetSDL() && GetSDL()->StartTextInput)
	{
		GetSDL()->StartTextInput();
	}

	if (CGameUIViewport::Get())
	{
		CGameUIViewport::Get()->PreventEscapeToShow(true);
	}
}

void RkHudChat::StopMessageMode()
{
	m_iMode = MM_NONE;

	if (m_elemChatInput)
	{
		m_elemChatInput->Blur();
	}

	if (m_pInstance)
	{
		m_pInstance->SetClass("chat_open", false);
		m_pInstance->DispatchEvent("chat_close", Rml::Dictionary());
	}

	if (m_bGrabbingInput)
	{
		RocketUIImpl::m_Instance.DenyInputToGame(false, "Hud_Chat");
		m_bGrabbingInput = false;
	}
	// HACKHACK: theres a conflict between vgui2 and rmlui sooo
	if (GetSDL() && GetSDL()->StartTextInput)
	{
		GetSDL()->StartTextInput();
	}

	if (CGameUIViewport::Get())
	{
		CGameUIViewport::Get()->PreventEscapeToShow(false);
	}
}

bool RkHudChat::ChatRaised() const
{
	return m_iMode != MM_NONE;
}

void RkHudChat::ClearChatHistory()
{
	if (!m_pInstance)
		return;

	Rml::ElementList chatEntries;
	m_pInstance->GetElementsByClassName(chatEntries, "chat_line");
	for (Rml::Element *elem : chatEntries)
	{
		if (elem->GetParentNode())
		{
			elem->GetParentNode()->RemoveChild(elem);
		}
	}
	m_iNumEntries = 0;
}

void RkHudChat::AddChatString(const char *username, const char *message, MessageSender sender)
{
	if (!m_pInstance || !m_elemChatLines)
		return;

	m_iNumEntries++;
	if (m_iNumEntries > rocket_hud_chat_max_entries.GetInt())
	{
		ClearChatHistory();
	}

	Rml::ElementPtr chatLine = m_pInstance->CreateElement("div");
	if (!chatLine)
		return;

	chatLine->SetClass("chat_line", true);

	float curtime = gHUD.m_flTime;
	if (curtime <= 0.0f)
		curtime = (float)RocketSystem::m_Instance.GetElapsedTime();
	chatLine->SetAttribute("data-time", std::to_string(curtime));

	char cleanUser[256] = {0};
	char cleanMsg[4096] = {0};

	if (username && username[0])
	{
		RemoveColorCodes(username, cleanUser, sizeof(cleanUser));
	}
	if (message && message[0])
	{
		RemoveColorCodes(message, cleanMsg, sizeof(cleanMsg));
	}

	if (cleanUser[0])
	{
		Rml::ElementPtr chatUsername = m_pInstance->CreateElement("span");
		if (chatUsername)
		{
			switch (sender)
			{
			case RkHudChat::SERVER:
				chatUsername->SetClass("chat_username_server", true);
				break;
			case RkHudChat::FRIEND:
				chatUsername->SetClass("chat_username_friend", true);
				break;
			case RkHudChat::FOE:
				chatUsername->SetClass("chat_username_foe", true);
				break;
			}
			Rml::String userText = cleanUser;
			userText += ": ";
			chatUsername->AppendChild(m_pInstance->CreateTextNode(userText));
			chatLine->AppendChild(std::move(chatUsername));
		}
	}

	if (cleanMsg[0])
	{
		Rml::ElementPtr chatMessage = m_pInstance->CreateElement("span");
		if (chatMessage)
		{
			chatMessage->SetClass("chat_message", true);
			chatMessage->AppendChild(m_pInstance->CreateTextNode(cleanMsg));
			chatLine->AppendChild(std::move(chatMessage));
		}
	}

	m_elemChatLines->AppendChild(std::move(chatLine));

	m_pInstance->UpdateDocument();
	m_elemChatLines->SetScrollTop(m_elemChatLines->GetScrollHeight() - m_elemChatLines->GetClientHeight());
}

void RkHudChat::AddChatString(const wchar_t *username, const wchar_t *message, MessageSender sender)
{
	char uBuf[256] = {0};
	char mBuf[2048] = {0};

	if (username)
	{
		Q_UnicodeToUTF8(username, uBuf, sizeof(uBuf));
	}
	if (message)
	{
		Q_UnicodeToUTF8(message, mBuf, sizeof(mBuf));
	}

	AddChatString(username ? uBuf : nullptr, message ? mBuf : nullptr, sender);
}
