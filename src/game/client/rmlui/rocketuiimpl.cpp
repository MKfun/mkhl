#include "rocketuiimpl.h"
#include "FileSystem.h"
#include "KeyValues.h"
#include "sdl_rt.h"
#include "utlbuffer.h"
#ifdef Debugger
#undef Debugger
#endif

#include "rocketsystem.h"
#include "rocketrenderer.h"
#include "rocketfilesystem.h"
#include "tier2/tier2.h"
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#pragma pop_macro("Assert")
#include "keydefs.h"

#include "rocketkeys.h"

#define GL_ALR_INCLUDED
RocketUIImpl RocketUIImpl::m_Instance;
// EXPOSE_SINGLE_INTERFACE_GLOBALVAR( RocketUIImpl, IRocketUI, ROCKETUI_INTERFACE_VERSION, RocketUIImpl::m_Instance )

ConVar rocket_enable( "rocket_enable", "1", 0, "Enables RocketUI" );
ConVar rocket_hud_scale("rocket_hud_scale", "1.0", FCVAR_ARCHIVE, "Hud scale modifier");
CON_COMMAND( rocket_reload, "Reloads all RocketUI Documents" )
{
    if( RocketUIImpl::m_Instance.ReloadDocuments() )
    {
        Msg("[RocketUI]Documents Reloaded.\n");
    }
    else
    {
        Msg("[RocketUI]Error reloading Documents!\n");
    }
}

CON_COMMAND( rocket_debug, "Open/Close the RocketUI Debugger" )
{
    RocketUIImpl::m_Instance.ToggleDebugger();
}



RocketUIImpl::RocketUIImpl() { }


Rml::Context* RocketUIImpl::AccessHudContext()
{
    return m_ctxHud;
}

Rml::Context* RocketUIImpl::AccessMenuContext()
{
    return m_ctxMenu;
}
typedef struct
{
    const char* path;
    const char* name;
} FontInfo;

void GetFontsFromConfig(const char* filename, CUtlVector<FontInfo> *finf)
{
    Msg("[RocketUI] Loading config: %s\n", filename);
    KeyValues *kv = new KeyValues("Fonts");
    if (!kv->LoadFromFile(g_pFullFileSystem, "rocketui/fonts.vdf", "GAME"))
    {

        printf("[RocketUI]no fonts.vdf found!\n");
        return;
    }
    for (KeyValues *it = kv->GetFirstSubKey(); it != NULL; it = it->GetNextKey())
    {
        FontInfo fi;
        fi.name = V_strdup(it->GetName());
        fi.path = V_strdup(it->GetString());
        finf->AddToTail(fi);
    }
    if (kv) kv->deleteThis();

}


bool ReadFile(const char* filepath, const char *pPath, CUtlBuffer &buf)
{
    bool bSuccess = 0;
    bool bBinary = !( buf.IsText() && !buf.ContainsCRLF() );

    FileHandle_t file = g_pFullFileSystem->Open(filepath, ( bBinary ) ? "rb" : "rt", pPath);
    if (file == FILESYSTEM_INVALID_HANDLE)
        return bSuccess;
    int nFileSize = g_pFullFileSystem->Size(file);
    buf.EnsureCapacity(nFileSize);
    int nBytesRead = g_pFullFileSystem->Read(buf.Base(), nFileSize, file);
    buf.SeekPut(CUtlBuffer::SEEK_HEAD, nBytesRead);
    g_pFullFileSystem->Close(file);
    bSuccess = 1;
    return bSuccess;
}

bool RocketUIImpl::LoadFont( const char *filepath, const char* fontName, const char *path )
{
    unsigned char *fontBuffer = NULL;
    CUtlBuffer font;
    unsigned int fontLen;

    if( !ReadFile( filepath, path, font ) )
    {
        fprintf(stderr, "[RocketUI]Failed to read %s font.\n", filepath );
        return false;
    }

    fontLen = font.Size() - 1;

    if( fontLen >= ( 8 * 1024 * 1024 ) )
    {
        fprintf(stderr, "[RocketUI]Font (%s) is over 8MB!(%d). Not Loading.\n", filepath, fontLen );
        return false;
    }

    fprintf(stderr, "[RocketUI]Font size (%d)\n", fontLen );

    fontBuffer = new unsigned char[ fontLen + 1 ];
    // Add to list of alloc'd fonts. Freetype will use this memory until we Shutdown.
    m_fontAllocs.AddToTail( fontBuffer );

    font.Get( fontBuffer, fontLen );
    Rml::Span<const Rml::byte> fontSpan(fontBuffer, fontLen);

    if( !Rml::LoadFontFace( fontSpan, fontName, Rml::Style::FontStyle::Normal, Rml::Style::FontWeight::Normal, false ) )
    {
        fprintf(stderr,  "[RocketUI]Failed to Initialize %s font\n", fontName );
        return false;
    }

    return true;
}


bool RocketUIImpl::LoadFonts()
{
    bool fontsOK = true;
    fontsOK &= LoadFont( "rocketui/fonts/Lato-Black.ttf", "Lato", "GAME" );
    CUtlVector<FontInfo> fontsVec;
    GetFontsFromConfig("rocketui/fonts.vdf", &fontsVec);
    for (int i = 0; i < fontsVec.Count(); i++ )
    {
        fontsOK &= LoadFont( fontsVec[i].path, fontsVec[i].name, "GAME" );
    }
    return fontsOK;
}
Rml::ElementDocument *LoadDocumentFile(Rml::Context *ctx, const char *tag, const char *pPath, const char *filepath)
{
    static char documentBuffer[ 4 * 1024 * 1024 ]; //4mb
    std::string documentStr;
    CUtlBuffer buffer;
    Rml::ElementDocument *document;

    if( !ReadFile( filepath, pPath, buffer ) )
    {
        fprintf(stderr, "[RocketUI]Failed to read file (%s)\n", filepath );
        return NULL;
    }
    buffer.GetString( documentBuffer );
    documentStr = documentBuffer;
    document = ctx->LoadDocumentFromMemory( documentStr );
    if( !document )
    {
        fprintf(stderr, "[RocketUI]Failed to load document from memory (%s)\n", filepath);
        return NULL;
    }
    fprintf(stderr, "[RocketUI]Document size (%d)\n", buffer.Size() - 1 );

    return document;
}

Rml::ElementDocument *RocketUIImpl::LoadDocumentFileIntoHud( const char *tag, const char *pPath, const char *filepath, documentReloadFuncs *m_pReloadDocFuncs )
{
    Rml::ElementDocument *document = LoadDocumentFile( m_ctxHud, tag, pPath, filepath );

    if( !document )
        return nullptr;

    // Need both
    if( m_pReloadDocFuncs->LoadDocument && m_pReloadDocFuncs->UnloadDocument )
    {
        m_documentReloadFuncs.AddToTail( m_pReloadDocFuncs );
    }

    return document;
}

Rml::ElementDocument *RocketUIImpl::LoadDocumentFileIntoMenu( const char *tag, const char *pPath, const char *filepath, documentReloadFuncs* m_pReloadDocumentFuncs )
{
    Rml::ElementDocument *document = LoadDocumentFile( m_ctxMenu, tag, pPath, filepath );

    if( !document )
        return nullptr;

    if( m_pReloadDocumentFuncs->LoadDocument && m_pReloadDocumentFuncs->UnloadDocument )
    {
        m_documentReloadFuncs.AddToTail( m_pReloadDocumentFuncs );
    }

    return document;
}

int RocketUIImpl::Init( void )
{
    // Create/Init the Rocket UI Library
    // Default width/height, these get updated in the DeviceCallbacks
    SCREENINFO m_scrinfo;
    m_scrinfo.iSize = sizeof(m_scrinfo);
    gEngfuncs.pfnGetScreenInfo(&m_scrinfo);
    int width = m_scrinfo.iWidth;
    int height = m_scrinfo.iHeight;

    RocketRender::m_Instance.SetScreenSize( width, height);
    Rml::SetFileInterface( &RocketFileSystem::m_Instance );
    Rml::SetRenderInterface( &RocketRender::m_Instance );
    Rml::SetSystemInterface( &RocketSystem::m_Instance );

    if ( !Rml::Initialise() )
    {
        Warning( "RocketUI: Initialise() failed!\n");
        return 0;
    }
    if( !LoadFonts() )
    {
        Warning( "RocketUI: Failed to load fonts.\n" );
        return 0;
    }

    m_ctxMenu = Rml::CreateContext("menu", Rml::Vector2i(width, height));
    m_ctxHud = Rml::CreateContext("hud", Rml::Vector2i(width, height));

    if ( !m_ctxMenu || !m_ctxHud )
    {
        Warning( "RocketUI: Failed to create Hud/Menu context\n" );
        Rml::Shutdown();
        return 0;
    }

    m_ctxMenu->SetDensityIndependentPixelRatio(1.0f );
    m_ctxHud->SetDensityIndependentPixelRatio(1.0f );

    return 1;
}

void RocketUIImpl::Shutdown()
{
    // Shutdown RocketUI. All contexts are destroyed on shutdown.
    Rml::Shutdown();

    // freetype FT_Done_Face has been called. Time to free fonts.
    for( int i = 0; i < m_fontAllocs.Count(); i++ )
    {
        unsigned char *fontAlloc = m_fontAllocs[i];
        delete[] fontAlloc;
    }

    m_ctxCurrent = NULL;

}

void RocketUIImpl::RunFrame(float time)
{
    m_fTime = time;

    // This is important. Update the current context 1x per frame.
    // This basically needs to be called whenever elements are added/changed/removed
    // I am calling it 1x per frame here instead of all over the place for simplicity and no overlap.
    if( m_ctxCurrent )
        m_ctxCurrent->Update();

	// DLLHACKHACKHACK: if we can't set DPI at ::Init(),
	// lets just observe convar there, the most hacky way.
	// But this has good side: u dont even need to rocket_reload
	// when changing scale, cuz this convar is observed.
	static float oldDP = 0;
	if (oldDP != rocket_hud_scale.GetFloat())
	{
		oldDP = rocket_hud_scale.GetFloat();
		AccessHudContext()->SetDensityIndependentPixelRatio(oldDP);
	}
}

void RocketUIImpl::DenyInputToGame( bool value, const char *why )
{
    if( value )
    {
        m_numInputConsumers++;
        m_inputConsumers.AddToTail( CUtlString( why ) );
    }
    else
    {
        m_numInputConsumers--;
        m_inputConsumers.FindAndRemove( CUtlString( why ) );
    }

    EnableCursor( (m_numInputConsumers > 0) );

    Msg("input Consumers[%d]: ", m_numInputConsumers);
    for( int i = 0; i < m_inputConsumers.Count(); i++ )
    {
        Msg("(%s) ", m_inputConsumers[i].Get() );
    }
    Msg("\n");
}

bool RocketUIImpl::IsConsumingInput()
{
    return ( m_numInputConsumers > 0 );
}

void RocketUIImpl::EnableCursor(bool state)
{

 Msg("Turnin %s the mouse\n", state ? "on" : "off" );


    gEngfuncs.pfnSetMouseEnable(!state);
    GetSDL()->SetRelativeMouseMode(state? SDL_FALSE : SDL_TRUE);
    m_bCursorVisible = state;
}

bool IsMouseCode(int code)
{
    return code == K_MOUSE1 ||
        code == K_MOUSE2 ||
        code == K_MOUSE3 ||
        code == K_MOUSE4 ||
        code == K_MOUSE5 ||
        code == K_MWHEELUP ||
        code == K_MWHEELDOWN;
}
// This function is an input hook.
// return true if we want to deny the game the input.
bool RocketUIImpl::HandleInputEvent(bool keyDown, int keyNumber, const char *bindName)
{
    // Haven't rendered our very first frame ever yet.
    if( !m_ctxCurrent )
        return false;

    // Always get the mouse location.
	int mx = 0, my = 0;
	GetSDL()->GetMouseState(&mx, &my);
	static Vector2D mousePos(0, 0);
    if(mousePos != Vector2D(mx, my))
    {
        // TODO update this with keymodifiers
        mousePos = Vector2D(mx, my);
        m_ctxCurrent->ProcessMouseMove( mx, my, 0 );
    }

    // // Some edge cases
    // if( event.m_nType == IE_ButtonPressed )
    // {
        // Check for debugger. Toggle on F8.
        if( keyDown && keyNumber == K_F8 )
        {
            ToggleDebugger();
            return true;
        }
        // The magical ESC key for the pause menu. The game handles this in an awful way
        // CSGO will open the pause menu for us the 1st time, but after that it fubars
        // In order to minimize this component from reaching into the gamecode,
        // The pause menu will register itself via RegisterPauseMenu while loading.
        // Kinda Hacky, but it is direct from keys.cpp and prevents the VGUI code from messing with it too much.
        if( keyNumber == K_ESCAPE )
        {
            // if( m_togglePauseMenuFunc && m_pEngine->IsInGame() )
            // {
                // m_togglePauseMenuFunc();
            // }
        }
    // }

    // Nothing wants input, skip.
    if( !IsConsumingInput() )
        return false;

    Rml::Input::KeyIdentifier key;
    char ascii;

    // switch( keyNumber )
    // {
    // case IE_ButtonDoubleClicked:
    if (keyDown)
    {
        //TODO add key modifiers
        if( IsMouseCode( keyNumber ) )
        {
            switch( keyNumber )
            {
            case K_MOUSE1:
                m_ctxCurrent->ProcessMouseButtonDown( 0, 0 );
                break;
            case K_MOUSE2:
                m_ctxCurrent->ProcessMouseButtonDown( 1, 0 );
                break;
            case K_MOUSE3:
                m_ctxCurrent->ProcessMouseButtonDown( 2, 0 );
                break;
            case K_MOUSE4:
                m_ctxCurrent->ProcessMouseButtonDown( 3, 0 );
                break;
            case K_MOUSE5:
                m_ctxCurrent->ProcessMouseButtonDown( 4, 0 );
                break;
            case K_MWHEELUP:
                m_ctxCurrent->ProcessMouseWheel( -1, 0 );
                break;
            case K_MWHEELDOWN:
                m_ctxCurrent->ProcessMouseWheel( 1, 0 );
                break;
            }
        }
        else
        {
            m_ctxCurrent->ProcessKeyDown( ButtonToRocketKey( keyNumber ), 0 );
        }
    }
    else
    {
        if( IsMouseCode( keyNumber ) )
        {
            switch( keyNumber )
            {
            case K_MOUSE1:
                m_ctxCurrent->ProcessMouseButtonUp( 0, 0 );
                break;
            case K_MOUSE2:
                m_ctxCurrent->ProcessMouseButtonUp( 1, 0 );
                break;
            case K_MOUSE3:
                m_ctxCurrent->ProcessMouseButtonUp( 2, 0 );
                break;
            case K_MOUSE4:
                m_ctxCurrent->ProcessMouseButtonUp( 3, 0 );
                break;
            case K_MOUSE5:
                m_ctxCurrent->ProcessMouseButtonUp( 4, 0 );
                break;
            }
        }
        else
        {
            m_ctxCurrent->ProcessKeyUp( ButtonToRocketKey( keyNumber ), 0 );
        }
    }
    return IsConsumingInput();
}

void SaveGLState();
void RestoreGLState();
void RocketUIImpl::RenderHUDFrame()
{
    if( !rocket_enable.GetBool() )
        return;

    m_ctxCurrent = m_ctxHud;

    RocketRender::m_Instance.PrepareGLState();
    SaveGLState();

    // m_ctxHud->Update();
    //m_ctxMenu->Update();

    m_ctxHud->Render();
    //m_ctxMenu->Render();
    RestoreGLState();
}

void RocketUIImpl::RenderMenuFrame()
{
    if( !rocket_enable.GetBool() )
        return;

    m_ctxCurrent = m_ctxMenu;

    SaveGLState();
    RocketRender::m_Instance.PrepareGLState();
    //glActiveTexture(GL_TEXTURE0);
    //TODO: don't update here. update only after input or new elements
    //m_ctxMenu->Update();

    m_ctxMenu->Render();

    RestoreGLState();
}

bool RocketUIImpl::ReloadDocuments()
{
    rocket_enable.SetValue( false );
    // Hacky, sleep for 100ms after disabling UI.
    // I dont feel like adding a mutex check every frame for something rarely used by devs
    ThreadSleep( 100 );

    CUtlVector<documentReloadFuncs*> copyOfPairs;

    // Copy the pairs into a local Vector( grug, copy constructor no work )
    // We want a copy because the loading functions will mess with our Vector when we call them.
    for( int i = 0; i < m_documentReloadFuncs.Count(); i++ )
    {
        copyOfPairs.AddToTail( m_documentReloadFuncs[i] );
    }

    // We can now empty the Main Vector since we are about to reload.
    m_documentReloadFuncs.Purge();

    // Go through the copy and reload
    for( int i = 0; i < copyOfPairs.Count(); i++ )
    {
        documentReloadFuncs *documentPair = copyOfPairs[i];
        // Unload...
        documentPair->UnloadDocument();
        documentPair->LoadDocument();
        // Load...
    }

    rocket_enable.SetValue( true );
    return true;
}

void RocketUIImpl::ToggleDebugger()
{
    static bool open = false;
    static bool firstTime = true;

    open = !open;

    if( !m_ctxCurrent )
        return;

    if( open )
    {
        if( firstTime )
        {
            if( Rml::Debugger::Initialise( m_ctxCurrent ) )
            {
                firstTime = false;
            }
            else
            {
                Msg("[RocketUI]Error Initializing Debugger\n");
                return;
            }
        }
        Msg("[RocketUI]Opening Debugger\n");
        if( !Rml::Debugger::SetContext( m_ctxCurrent ) )
        {
            Msg("[RocketUI]Error setting context!\n");
            return;
        }
        m_isDebuggerOpen = true;
        Rml::Debugger::SetVisible( true );
        DenyInputToGame( true, "RocketUI Debugger" );
    }
    else
    {
        Msg("[RocketUI]Closing Debugger\n");
        Rml::Debugger::SetVisible( false );
        m_isDebuggerOpen = false;
        DenyInputToGame( false, "RocketUI Debugger" );
    }
}
