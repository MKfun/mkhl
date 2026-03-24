#include "gamemenuitem.h"
#include "KeyValues.h"

CGameMenuItem::CGameMenuItem( vgui2::Panel *pParent, const char *panelName, const char *text, vgui2::Panel *pActionSignalTarget, IFileMenuCallbacks *pFileMenuCallback ) :
    BaseClass( pParent, panelName, text, pActionSignalTarget ), m_pFileMenuCallback( pFileMenuCallback )
{
    Assert( pFileMenuCallback );
    AddMenuItem( "new", "#GameNew", new KeyValues( "OnNew" ), pActionSignalTarget, NULL, "file_new" );
    AddMenuItem( "open", "#ToolFileOpen", new KeyValues( "OnOpen" ), pActionSignalTarget, NULL, "file_open"  );
    AddMenuItem( "save", "#ToolFileSave", new KeyValues( "OnSave" ), pActionSignalTarget, NULL, "file_save"  );
    AddMenuItem( "saveas", "#ToolFileSaveAs", new KeyValues( "OnSaveAs" ), pActionSignalTarget  );
    AddMenuItem( "close", "#ToolFileClose", new KeyValues( "OnClose" ), pActionSignalTarget  );
    AddSeparator();

}
