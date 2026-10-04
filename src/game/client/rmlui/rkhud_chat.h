#ifndef RKHUD_CHAT_H
#define RKHUD_CHAT_H

#include "rmlui/rkhud_elem_interface.h"
#include <rmlui/rocketuiimpl.h>

#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#pragma pop_macro("Assert")


class RkHudChat : public CRocketHudElem
{
public:
	enum MessageSender
	{
		SERVER,
		FRIEND,
		FOE,
	};

	explicit RkHudChat(const char *value);
	virtual ~RkHudChat();
	static RkHudChat m_Instance;

	// Overrides from CRocketHudElem
	void LevelInit(void) override;
	virtual void LevelShutdown(void) override;
	virtual void SetActive(bool bActive) override;
	virtual bool ShouldDraw(void) override;
	void ShowPanel(bool bShow, bool force) override;
	virtual void Update(void) override;

	bool ChatRaised() const;

	void AddChatString(const char *username, const char *message, MessageSender sender);
	void AddChatString(const wchar_t *username, const wchar_t *message, MessageSender sender);
	void ClearChatHistory();

	// Starts the typing sequence.
	void StartMessageMode(int mode);
	void StopMessageMode();
	int GetMessageMode() const { return m_iMode; }

	Rml::ElementDocument *m_pInstance = nullptr;
	// Precached elements from the instance.
	Rml::Element *m_elemChatLines = nullptr;
	Rml::Element *m_elemChatInput = nullptr;

	bool m_bVisible = false;
	bool m_bGrabbingInput = false;
	int m_iMode = 0;
	int m_iNumEntries = 0;
};

#endif // RKHUD_CHAT_H
