#ifndef GAMEMENUBAR_H
#define GAMEMENUBAR_H

#include "vgui_controls/MenuBar.h"

class CGameMenuBar : public vgui2::MenuBar
{
	DECLARE_CLASS_SIMPLE(CGameMenuBar, vgui2::MenuBar);

public:
	CGameMenuBar(vgui2::Panel *parent);
	void UpdateMenuItemState(bool isInGame, bool isMulti);
	void AddMenuEx(const char *name, vgui2::Menu *menu, bool bAlignRight = false);
	virtual void PerformLayout() override;

private:
	CUtlVector<vgui2::Panel *> m_RightPanels;
	CUtlVector<vgui2::Panel *> m_LeftPanels;
};

#endif // GAMEMENUBAR_H
