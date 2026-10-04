#include "rocketuiimpl.h"
#include "FileSystem.h"
#include "KeyValues.h"
#include "RmlUi/Lua/Lua.h"
#include "sdl_rt.h"
#include "utlbuffer.h"
#ifdef Debugger
#undef Debugger
#endif

#include "rocketsystem.h"
#include "rocketrenderer.h"
#include "rocketfilesystem.h"
#define TIER2_GAMEUI_INTERNALS
#include "tier2/tier2.h"
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#pragma pop_macro("Assert")
#include "keydefs.h"
#include "rocketkeys.h"
#include "rkhud_chat.h"
#include "vgui/ILocalize.h"
#include "tier1/strtools.h"
extern lua_State *gLuaState;

#define GL_ALR_INCLUDED
RocketUIImpl RocketUIImpl::m_Instance;
// EXPOSE_SINGLE_INTERFACE_GLOBALVAR( RocketUIImpl, IRocketUI, ROCKETUI_INTERFACE_VERSION, RocketUIImpl::m_Instance )

static bool s_bHasSDLEventWatch = false;

static int SDLCALL RocketUI_SDLEventWatcher(void *userdata, SDL_Event *event)
{
    if (event && event->type == SDL_TEXTINPUT && RocketUIImpl::m_Instance.IsConsumingInput())
    {
        if (event->text.text[0] != '\0' && event->text.text[0] != '\r' && event->text.text[0] != '\n')
        {
            Rml::Context *ctx = RocketUIImpl::m_Instance.GetActiveInputContext();
            if (ctx)
            {
                ctx->ProcessTextInput(Rml::String(event->text.text));
            }
        }
    }
    return 1;
}

Rml::Context *RocketUIImpl::GetActiveInputContext()
{
    if (RkHudChat::m_Instance.ChatRaised() && m_ctxHud)
    {
        return m_ctxHud;
    }
    if (m_ctxCurrent)
    {
        return m_ctxCurrent;
    }
    return m_ctxHud;
}

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
    int fontLen;

    if( !ReadFile( filepath, path, font ) )
    {
        fprintf(stderr, "[RocketUI]Failed to read %s font.\n", filepath );
        return false;
    }

    fontLen = font.TellPut();

    if( fontLen <= 0 || fontLen >= ( 8 * 1024 * 1024 ) )
    {
        fprintf(stderr, "[RocketUI]Font (%s) has invalid size (%d). Not Loading.\n", filepath, fontLen );
        return false;
    }

    fprintf(stderr, "[RocketUI]Font %s size (%d)\n", filepath, fontLen );

    fontBuffer = new unsigned char[ fontLen ];
    // Add to list of alloc'd fonts. Freetype will use this memory until we Shutdown.
    m_fontAllocs.AddToTail( fontBuffer );

    memcpy( fontBuffer, font.Base(), fontLen );
    Rml::Span<const Rml::byte> fontSpan(fontBuffer, fontLen);

    Rml::Style::FontStyle fontStyle = (V_stristr(filepath, "italic") != NULL) ? Rml::Style::FontStyle::Italic : Rml::Style::FontStyle::Normal;

    if( !Rml::LoadFontFace( fontSpan, fontName, fontStyle, Rml::Style::FontWeight::Auto, false ) )
    {
        fprintf(stderr,  "[RocketUI]Failed to Initialize %s font\n", fontName );
        return false;
    }

    return true;
}


bool RocketUIImpl::LoadFonts()
{
    bool fontsOK = true;
    CUtlVector<FontInfo> fontsVec;
    GetFontsFromConfig("rocketui/fonts.vdf", &fontsVec);
    if (fontsVec.Count() == 0)
    {
        fontsOK &= LoadFont( "rocketui/fonts/Lato-Black.ttf", "Lato", "GAME" );
    }
    else
    {
        for (int i = 0; i < fontsVec.Count(); i++ )
        {
            fontsOK &= LoadFont( fontsVec[i].path, fontsVec[i].name, "GAME" );
        }
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
	Rml::Lua::Initialise(gLuaState);
	if (!LoadFonts())
	{
		Warning("RocketUI: Failed to load fonts.\n");
		return 0;
	}

	m_ctxMenu = Rml::CreateContext("menu", Rml::Vector2i(width, height));
	m_ctxHud = Rml::CreateContext("hud", Rml::Vector2i(width, height));

	if (!m_ctxMenu || !m_ctxHud)
	{
		Warning("RocketUI: Failed to create Hud/Menu context\n");
		Rml::Shutdown();
		return 0;
	}

	m_ctxMenu->SetDensityIndependentPixelRatio(1.0f);
	m_ctxHud->SetDensityIndependentPixelRatio(1.0f);

	if (!s_bHasSDLEventWatch && GetSDL() && GetSDL()->AddEventWatch)
	{
		GetSDL()->AddEventWatch(RocketUI_SDLEventWatcher, nullptr);
		s_bHasSDLEventWatch = true;
	}

	return 1;
}

void RocketUIImpl::Shutdown()
{
    if (s_bHasSDLEventWatch && GetSDL() && GetSDL()->DelEventWatch)
    {
        GetSDL()->DelEventWatch(RocketUI_SDLEventWatcher, nullptr);
        s_bHasSDLEventWatch = false;
    }

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
    if( m_ctxHud )
        m_ctxHud->Update();
    if( m_ctxMenu )
        m_ctxMenu->Update();

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
        if (m_numInputConsumers > 0)
            m_numInputConsumers--;
        m_inputConsumers.FindAndRemove( CUtlString( why ) );
    }

    EnableCursor( (m_numInputConsumers > 0) );

    // Always ensure text input is started so SDL2 generates SDL_TEXTINPUT events for both RocketUI and VGUI2!
    if (GetSDL() && GetSDL()->StartTextInput)
    {
        GetSDL()->StartTextInput();
    }

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
    // Check which context should receive input.
    Rml::Context *ctx = GetActiveInputContext();

    if (!ctx)
        return false;

    // Track key modifiers
    static int s_fallbackKeyModifiers = 0;
    int keyModifierState = 0;
    if (GetSDL() && GetSDL()->GetModState)
    {
        SDL_Keymod mod = GetSDL()->GetModState();
        if (mod & KMOD_CTRL)  keyModifierState |= Rml::Input::KM_CTRL;
        if (mod & KMOD_SHIFT) keyModifierState |= Rml::Input::KM_SHIFT;
        if (mod & KMOD_ALT)   keyModifierState |= Rml::Input::KM_ALT;
        if (mod & KMOD_GUI)   keyModifierState |= Rml::Input::KM_META;
        if (mod & KMOD_CAPS)  keyModifierState |= Rml::Input::KM_CAPSLOCK;
        if (mod & KMOD_NUM)   keyModifierState |= Rml::Input::KM_NUMLOCK;
    }
    else
    {
        keyModifierState = s_fallbackKeyModifiers;
    }

    if (keyDown)
    {
        if (keyNumber == K_SHIFT) { keyModifierState |= Rml::Input::KM_SHIFT; s_fallbackKeyModifiers |= Rml::Input::KM_SHIFT; }
        else if (keyNumber == K_CTRL) { keyModifierState |= Rml::Input::KM_CTRL; s_fallbackKeyModifiers |= Rml::Input::KM_CTRL; }
        else if (keyNumber == K_ALT) { keyModifierState |= Rml::Input::KM_ALT; s_fallbackKeyModifiers |= Rml::Input::KM_ALT; }
        else if (keyNumber == K_CAPSLOCK) { keyModifierState ^= Rml::Input::KM_CAPSLOCK; s_fallbackKeyModifiers ^= Rml::Input::KM_CAPSLOCK; }
    }
    else
    {
        if (keyNumber == K_SHIFT) { s_fallbackKeyModifiers &= ~Rml::Input::KM_SHIFT; keyModifierState &= ~Rml::Input::KM_SHIFT; }
        else if (keyNumber == K_CTRL) { s_fallbackKeyModifiers &= ~Rml::Input::KM_CTRL; keyModifierState &= ~Rml::Input::KM_CTRL; }
        else if (keyNumber == K_ALT) { s_fallbackKeyModifiers &= ~Rml::Input::KM_ALT; keyModifierState &= ~Rml::Input::KM_ALT; }
    }

    // Always get the mouse location.
    int mx = 0, my = 0;
    GetSDL()->GetMouseState(&mx, &my);
    static Vector2D mousePos(0, 0);
    if (mousePos != Vector2D(mx, my))
    {
        mousePos = Vector2D(mx, my);
        ctx->ProcessMouseMove(mx, my, keyModifierState);
    }

    // Check for debugger. Toggle on F8.
    if (keyDown && keyNumber == K_F8)
    {
        ToggleDebugger();
        return true;
    }

    // If user presses toggleconsole or tilde/grave, close chat if open and pass key to engine
    if (bindName && (!stricmp(bindName, "toggleconsole") || !stricmp(bindName, "cancelselect")))
    {
        if (RkHudChat::m_Instance.ChatRaised())
        {
            RkHudChat::m_Instance.StopMessageMode();
        }
        return false;
    }
    if (keyNumber == '`' || keyNumber == '~')
    {
        if (RkHudChat::m_Instance.ChatRaised())
        {
            RkHudChat::m_Instance.StopMessageMode();
        }
        return false;
    }

    // Nothing wants input, skip.
    if (!IsConsumingInput())
        return false;

    if (keyDown)
    {
        if (IsMouseCode(keyNumber))
        {
            switch (keyNumber)
            {
            case K_MOUSE1:
                ctx->ProcessMouseButtonDown(0, keyModifierState);
                break;
            case K_MOUSE2:
                ctx->ProcessMouseButtonDown(1, keyModifierState);
                break;
            case K_MOUSE3:
                ctx->ProcessMouseButtonDown(2, keyModifierState);
                break;
            case K_MOUSE4:
                ctx->ProcessMouseButtonDown(3, keyModifierState);
                break;
            case K_MOUSE5:
                ctx->ProcessMouseButtonDown(4, keyModifierState);
                break;
            case K_MWHEELUP:
                ctx->ProcessMouseWheel(-1, keyModifierState);
                break;
            case K_MWHEELDOWN:
                ctx->ProcessMouseWheel(1, keyModifierState);
                break;
            }
        }
        else
        {
            Rml::Input::KeyIdentifier key = ButtonToRocketKey(keyNumber);
            ctx->ProcessKeyDown(key, keyModifierState);

            // Generate text input if Ctrl, Alt, Meta are not held
            // When SDL event watcher is active, SDL_TEXTINPUT provides all text input (with proper UTF-8 layout translation)
            if (!s_bHasSDLEventWatch && (keyModifierState & (Rml::Input::KM_CTRL | Rml::Input::KM_ALT | Rml::Input::KM_META)) == 0)
            {
                Rml::Character c = GetCharacterCode(key, keyModifierState);
                if (c != Rml::Character::Null && (char32_t)c >= 32 && (char32_t)c != 127 && (char32_t)c != '\n' && (char32_t)c != '\r')
                {
                    ctx->ProcessTextInput(c);
                }
                else if (keyNumber >= 32 && keyNumber <= 126)
                {
                    ctx->ProcessTextInput((char)keyNumber);
                }
                else if ((unsigned char)keyNumber >= 128 && keyNumber <= 255)
                {
                    bool isSpecial = (keyNumber >= K_UPARROW && keyNumber <= K_WIN) ||
                                     (keyNumber >= K_JOY1 && keyNumber <= K_MOUSE5) ||
                                     (keyNumber == K_PAUSE);
                    if (!isSpecial && g_pVGuiLocalize)
                    {
                        char ansi[2] = { (char)keyNumber, '\0' };
                        wchar_t unicode[2] = { 0, 0 };
                        g_pVGuiLocalize->ConvertANSIToUnicode(ansi, unicode, sizeof(unicode));
                        if (unicode[0] != 0)
                        {
                            char utf8[8] = { 0 };
                            Q_UnicodeToUTF8(unicode, utf8, sizeof(utf8));
                            ctx->ProcessTextInput(Rml::String(utf8));
                        }
                    }
                }
            }
        }
    }
    else
    {
        if (IsMouseCode(keyNumber))
        {
            switch (keyNumber)
            {
            case K_MOUSE1:
                ctx->ProcessMouseButtonUp(0, keyModifierState);
                break;
            case K_MOUSE2:
                ctx->ProcessMouseButtonUp(1, keyModifierState);
                break;
            case K_MOUSE3:
                ctx->ProcessMouseButtonUp(2, keyModifierState);
                break;
            case K_MOUSE4:
                ctx->ProcessMouseButtonUp(3, keyModifierState);
                break;
            case K_MOUSE5:
                ctx->ProcessMouseButtonUp(4, keyModifierState);
                break;
            }
        }
        else
        {
            ctx->ProcessKeyUp(ButtonToRocketKey(keyNumber), keyModifierState);
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

    SaveGLState();
    RocketRender::m_Instance.PrepareGLState();

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
