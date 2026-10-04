#include "rocketsystem.h"

#include "rocketuiimpl.h"
#include "sdl_rt.h"

#include "tier0/platform.h"
#include "tier2/tier2.h"
#include "vgui/ISystem.h"

RocketSystem RocketSystem::m_Instance;

double RocketSystem::GetElapsedTime()
{
    return Plat_FloatTime();
}

bool RocketSystem::LogMessage(Rml::Log::Type type, const Rml::String &message)
{
    bool ret = false;
    if( type == Rml::Log::LT_ERROR )
        ret = true;

    //FIXME: an actual logging function that shows up in the terminal. Source engine has like 20+
    fprintf( stderr, "[RocketUI]%s\n", message.c_str() );

    return ret;
}

void RocketSystem::SetClipboardText(const Rml::String& text)
{
    if (GetSDL() && GetSDL()->SetClipboardText)
    {
        GetSDL()->SetClipboardText(text.c_str());
        return;
    }
    if (g_pVGuiSystem)
    {
        g_pVGuiSystem->SetClipboardText( text.c_str(), text.size() );
    }
}

void RocketSystem::GetClipboardText(Rml::String& text)
{
    if (GetSDL() && GetSDL()->GetClipboardText)
    {
        char *raw_text = GetSDL()->GetClipboardText();
        if (raw_text)
        {
            text = raw_text;
            if (GetSDL()->Free)
                GetSDL()->Free(raw_text);
            return;
        }
    }
    if (g_pVGuiSystem)
    {
        char buffer[1024];
        buffer[0] = '\0';
        g_pVGuiSystem->GetClipboardText(0, buffer, sizeof(buffer) );
        text = buffer;
    }
}

void RocketSystem::ActivateKeyboard(Rml::Vector2f caret_position, float line_height)
{
    (void)caret_position;
    (void)line_height;
    if (GetSDL() && GetSDL()->StartTextInput)
    {
        GetSDL()->StartTextInput();
    }
}

void RocketSystem::DeactivateKeyboard()
{
    // Do not call SDL_StopTextInput() globally.
    // The engine and VGUI2 (console, dialogs) require SDL text input to stay active
    // in order to receive SDL_TEXTINPUT events for typing.
    if (GetSDL() && GetSDL()->StartTextInput)
    {
        GetSDL()->StartTextInput();
    }
}

