#ifndef GAMEMENUITEM_H
#define GAMEMENUITEM_H

#include "vgui_controls/MenuButton.h"
#include "gameuimenubutton.h"
class IFileMenuCallbacks
{
public:
    enum MenuItems_t
    {
        GAME_NEW	= 0x01,
        FILE_LOAD	= 0x02,
        FILE_SAVE	= 0x04,

        FILE_ALL = 0xFFFFFFFF
    };

    // Logically OR together all items that should be enabled
    virtual int	 GetFileMenuItemsEnabled( ) = 0;

    // Add recent files to the menu passed in
    virtual void AddRecentFilesToMenu( vgui2::Menu *menu ) = 0;

    // Get the perforce file name (to set the various perforce menu options)
    virtual bool GetPerforceFileName( char *pFileName, int nMaxLen ) = 0;

    // Gets the root vgui panel
    virtual vgui2::Panel *GetRootPanel() = 0;
};

class CGameMenuItem : public CToolMenuButton
{
    DECLARE_CLASS_SIMPLE( CGameMenuItem, CToolMenuButton);
public:
    CGameMenuItem( vgui2::Panel *pParent, const char *panelName, const char *text, vgui2::Panel *pActionSignalTarget, IFileMenuCallbacks *pFileMenuCallback );
    //virtual void OnShowMenu( vgui2::Menu *menu );
private:
    IFileMenuCallbacks *m_pFileMenuCallback;
};

CGameMenuItem* CreateGameMenuButton( vgui2::Panel *parent, const char *panelName,
    const char *text, vgui2::Panel *pActionTarget, IFileMenuCallbacks *pCallbacks );


#endif // GAMEMENUITEM_H
