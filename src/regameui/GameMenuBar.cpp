#include "GameMenuBar.h"
#include "EngineInterface.h"
#include "KeyValues.h"
#include "vgui_controls/MenuButton.h"
#include "vgui_controls/MenuItem.h"

CGameMenuBar::CGameMenuBar(vgui2::Panel *parent)
    : BaseClass(parent, "MainMenuBar")
{
}

void CGameMenuBar::UpdateMenuItemState(bool isInGame, bool isMulti)
{
	// Проходим по всем кнопкам меню в MenuBar (Файл, Настройки и т.д.)
	for (int i = 0; i < GetChildCount(); i++)
	{
		vgui2::MenuButton *pMenuButton = dynamic_cast<vgui2::MenuButton *>(GetChild(i));
		if (!pMenuButton)
			continue;

		vgui2::Menu *pMenu = pMenuButton->GetMenu();
		if (!pMenu)
			continue;

		for (int j = 0; j < pMenu->GetChildCount(); j++)
		{
			vgui2::MenuItem *menuItem = dynamic_cast<vgui2::MenuItem *>(pMenu->GetChild(j));

			if (menuItem)
			{

				bool shouldBeVisible = true;
				KeyValues *kv = menuItem->GetUserData();

				if (!kv)
					continue;

				if (!isInGame && kv->GetInt("OnlyInGame"))
				{
					shouldBeVisible = false;
				}
				else if (isMulti && kv->GetInt("notmulti"))
				{
					shouldBeVisible = false;
				}
				else if (isInGame && !isMulti && kv->GetInt("notsingle"))
				{
					shouldBeVisible = false;
				}
				else if (isInGame && kv->GetInt("OnlyInMenu"))
				{
					shouldBeVisible = false;
				}

				menuItem->SetVisible(shouldBeVisible);
			}
		}
	}

	InvalidateLayout();
}

void CGameMenuBar::AddMenuEx(const char *name, vgui2::Menu *menu, bool bAlignRight)
{
	BaseClass::AddMenu(name, menu);

	if (bAlignRight && GetChildCount() > 0)
	{
		vgui2::Panel *pNewButton = GetChild(GetChildCount() - 1);
		m_RightPanels.AddToTail(pNewButton);
	}
}

void CGameMenuBar::PerformLayout()
{
	BaseClass::PerformLayout();
	int leftX = 4;
	int rightX = GetWide() - 4;
	int gap = 4;
	int childY = 2;

	for (int i = 0; i < m_LeftPanels.Count(); i++)
	{
		vgui2::Panel *pChild = m_LeftPanels[i];
		if (pChild && pChild->IsVisible())
		{
			pChild->SetPos(leftX, childY);
			leftX += pChild->GetWide() + gap;
		}
	}

	for (int i = 0; i < m_RightPanels.Count(); i++)
	{
		vgui2::Panel *pChild = m_RightPanels[i];
		if (pChild && pChild->IsVisible())
		{
			rightX -= pChild->GetWide();
			pChild->SetPos(rightX, childY);
			rightX -= gap;
		}
	}
}
