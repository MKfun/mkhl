#pragma once
#pragma push_macro("Assert")
#undef Assert
#include <RmlUi/Core/Input.h>
#pragma pop_macro("Assert")
#include "keydefs.h"
#include "vgui/KeyCode.h"

// Keycode converter helper.
// Keycode converter helper.
inline Rml::Input::KeyIdentifier ButtonToRocketKey( int button )
{
    using namespace Rml::Input;

    // ASCII lower-case letters
    if (button >= 'a' && button <= 'z')
        return (KeyIdentifier)(KI_A + (button - 'a'));
    // ASCII upper-case letters
    if (button >= 'A' && button <= 'Z')
        return (KeyIdentifier)(KI_A + (button - 'A'));
    // ASCII digits
    if (button >= '0' && button <= '9')
        return (KeyIdentifier)(KI_0 + (button - '0'));

    switch (button)
    {
    // ASCII punctuation / symbols
    case ' ':
        return KI_SPACE;
    case ';': case ':':
        return KI_OEM_1;
    case '=': case '+':
        return KI_OEM_PLUS;
    case ',': case '<':
        return KI_OEM_COMMA;
    case '-': case '_':
        return KI_OEM_MINUS;
    case '.': case '>':
        return KI_OEM_PERIOD;
    case '/': case '?':
        return KI_OEM_2;
    case '`': case '~':
        return KI_OEM_3;
    case '[': case '{':
        return KI_OEM_4;
    case '\\': case '|':
        return KI_OEM_5;
    case ']': case '}':
        return KI_OEM_6;
    case '\'': case '\"':
        return KI_OEM_7;
    case ')':
        return KI_0;
    case '!':
        return KI_1;
    case '@':
        return KI_2;
    case '#':
        return KI_3;
    case '$':
        return KI_4;
    case '%':
        return KI_5;
    case '^':
        return KI_6;
    case '&':
        return KI_7;
    case '*':
        return KI_8;
    case '(':
        return KI_9;

    // GoldSrc keydefs
    case K_ENTER:
        return KI_RETURN;
    case K_ESCAPE:
        return KI_ESCAPE;
    case K_BACKSPACE:
        return KI_BACK;
    case K_TAB:
        return KI_TAB;
    case K_UPARROW:
        return KI_UP;
    case K_DOWNARROW:
        return KI_DOWN;
    case K_LEFTARROW:
        return KI_LEFT;
    case K_RIGHTARROW:
        return KI_RIGHT;
    case K_ALT:
        return KI_LMENU;
    case K_CTRL:
        return KI_LCONTROL;
    case K_SHIFT:
        return KI_LSHIFT;
    case K_CAPSLOCK:
        return KI_CAPITAL;
    case K_WIN:
        return KI_LWIN;
    case K_INS:
        return KI_INSERT;
    case K_DEL:
        return KI_DELETE;
    case K_HOME:
        return KI_HOME;
    case K_END:
        return KI_END;
    case K_PGUP:
        return KI_PRIOR;
    case K_PGDN:
        return KI_NEXT;
    case K_PAUSE:
        return KI_PAUSE;

    case K_F1:
        return KI_F1;
    case K_F2:
        return KI_F2;
    case K_F3:
        return KI_F3;
    case K_F4:
        return KI_F4;
    case K_F5:
        return KI_F5;
    case K_F6:
        return KI_F6;
    case K_F7:
        return KI_F7;
    case K_F8:
        return KI_F8;
    case K_F9:
        return KI_F9;
    case K_F10:
        return KI_F10;
    case K_F11:
        return KI_F11;
    case K_F12:
        return KI_F12;

    case K_KP_HOME:
        return KI_NUMPAD7;
    case K_KP_UPARROW:
        return KI_NUMPAD8;
    case K_KP_PGUP:
        return KI_NUMPAD9;
    case K_KP_LEFTARROW:
        return KI_NUMPAD4;
    case K_KP_5:
        return KI_NUMPAD5;
    case K_KP_RIGHTARROW:
        return KI_NUMPAD6;
    case K_KP_END:
        return KI_NUMPAD1;
    case K_KP_DOWNARROW:
        return KI_NUMPAD2;
    case K_KP_PGDN:
        return KI_NUMPAD3;
    case K_KP_ENTER:
        return KI_NUMPADENTER;
    case K_KP_INS:
        return KI_NUMPAD0;
    case K_KP_DEL:
        return KI_DECIMAL;
    case K_KP_SLASH:
        return KI_DIVIDE;
    case K_KP_MINUS:
        return KI_SUBTRACT;
    case K_KP_PLUS:
        return KI_ADD;
    case K_KP_MUL:
        return KI_MULTIPLY;

    default:
        break;
    }

    // VGUI KeyCodes fallback
    if (button >= vgui2::KEY_0 && button <= vgui2::KEY_9)
        return (KeyIdentifier)(KI_0 + (button - vgui2::KEY_0));
    if (button >= vgui2::KEY_A && button <= vgui2::KEY_Z)
        return (KeyIdentifier)(KI_A + (button - vgui2::KEY_A));
    if (button >= vgui2::KEY_PAD_0 && button <= vgui2::KEY_PAD_9)
        return (KeyIdentifier)(KI_NUMPAD0 + (button - vgui2::KEY_PAD_0));

    switch (button)
    {
    case vgui2::KEY_PAD_DIVIDE:
        return KI_DIVIDE;
    case vgui2::KEY_PAD_MULTIPLY:
        return KI_MULTIPLY;
    case vgui2::KEY_PAD_MINUS:
        return KI_SUBTRACT;
    case vgui2::KEY_PAD_PLUS:
        return KI_ADD;
    case vgui2::KEY_PAD_ENTER:
        return KI_NUMPADENTER;
    case vgui2::KEY_PAD_DECIMAL:
        return KI_DECIMAL;
    case vgui2::KEY_LBRACKET:
        return KI_OEM_4;
    case vgui2::KEY_RBRACKET:
        return KI_OEM_6;
    case vgui2::KEY_SEMICOLON:
        return KI_OEM_1;
    case vgui2::KEY_APOSTROPHE:
        return KI_OEM_7;
    case vgui2::KEY_BACKQUOTE:
        return KI_OEM_3;
    case vgui2::KEY_COMMA:
        return KI_OEM_COMMA;
    case vgui2::KEY_PERIOD:
        return KI_OEM_PERIOD;
    case vgui2::KEY_SLASH:
        return KI_OEM_2;
    case vgui2::KEY_BACKSLASH:
        return KI_OEM_5;
    case vgui2::KEY_MINUS:
        return KI_OEM_MINUS;
    case vgui2::KEY_EQUAL:
        return KI_OEM_PLUS;
    case vgui2::KEY_ENTER:
        return KI_RETURN;
    case vgui2::KEY_SPACE:
        return KI_SPACE;
    case vgui2::KEY_BACKSPACE:
        return KI_BACK;
    case vgui2::KEY_TAB:
        return KI_TAB;
    case vgui2::KEY_CAPSLOCK:
        return KI_CAPITAL;
    case vgui2::KEY_NUMLOCK:
        return KI_NUMLOCK;
    case vgui2::KEY_ESCAPE:
        return KI_ESCAPE;
    case vgui2::KEY_SCROLLLOCK:
        return KI_SCROLL;
    case vgui2::KEY_INSERT:
        return KI_INSERT;
    case vgui2::KEY_DELETE:
        return KI_DELETE;
    case vgui2::KEY_HOME:
        return KI_HOME;
    case vgui2::KEY_END:
        return KI_END;
    case vgui2::KEY_PAGEUP:
        return KI_PRIOR;
    case vgui2::KEY_PAGEDOWN:
        return KI_NEXT;
    case vgui2::KEY_LSHIFT:
        return KI_LSHIFT;
    case vgui2::KEY_RSHIFT:
        return KI_RSHIFT;
    case vgui2::KEY_LALT:
        return KI_LMENU;
    case vgui2::KEY_RALT:
        return KI_RMENU;
    case vgui2::KEY_LCONTROL:
        return KI_LCONTROL;
    case vgui2::KEY_RCONTROL:
        return KI_RCONTROL;
    case vgui2::KEY_LWIN:
        return KI_LWIN;
    case vgui2::KEY_RWIN:
        return KI_RWIN;
    case vgui2::KEY_APP:
        return KI_APPS;
    case vgui2::KEY_UP:
        return KI_UP;
    case vgui2::KEY_LEFT:
        return KI_LEFT;
    case vgui2::KEY_DOWN:
        return KI_DOWN;
    case vgui2::KEY_RIGHT:
        return KI_RIGHT;
    default:
        break;
    }

    return KI_UNKNOWN;
}

/**
    This map contains 4 different mappings from key identifiers to character codes. Each entry represents a different
    combination of shift and capslock state.
 */

static const char ascii_map[4][51] =
    {
        // shift off and capslock off
        {
            0,
            ' ',
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            'a',
            'b',
            'c',
            'd',
            'e',
            'f',
            'g',
            'h',
            'i',
            'j',
            'k',
            'l',
            'm',
            'n',
            'o',
            'p',
            'q',
            'r',
            's',
            't',
            'u',
            'v',
            'w',
            'x',
            'y',
            'z',
            ';',
            '=',
            ',',
            '-',
            '.',
            '/',
            '`',
            '[',
            '\\',
            ']',
            '\'',
            0,
            0
        },

        // shift on and capslock off
        {
            0,
            ' ',
            ')',
            '!',
            '@',
            '#',
            '$',
            '%',
            '^',
            '&',
            '*',
            '(',
            'A',
            'B',
            'C',
            'D',
            'E',
            'F',
            'G',
            'H',
            'I',
            'J',
            'K',
            'L',
            'M',
            'N',
            'O',
            'P',
            'Q',
            'R',
            'S',
            'T',
            'U',
            'V',
            'W',
            'X',
            'Y',
            'Z',
            ':',
            '+',
            '<',
            '_',
            '>',
            '?',
            '~',
            '{',
            '|',
            '}',
            '"',
            0,
            0
        },

        // shift on and capslock on
        {
            0,
            ' ',
            ')',
            '!',
            '@',
            '#',
            '$',
            '%',
            '^',
            '&',
            '*',
            '(',
            'a',
            'b',
            'c',
            'd',
            'e',
            'f',
            'g',
            'h',
            'i',
            'j',
            'k',
            'l',
            'm',
            'n',
            'o',
            'p',
            'q',
            'r',
            's',
            't',
            'u',
            'v',
            'w',
            'x',
            'y',
            'z',
            ':',
            '+',
            '<',
            '_',
            '>',
            '?',
            '~',
            '{',
            '|',
            '}',
            '"',
            0,
            0
        },

        // shift off and capslock on
        {
            0,
            ' ',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            '0',
            'A',
            'B',
            'C',
            'D',
            'E',
            'F',
            'G',
            'H',
            'I',
            'J',
            'K',
            'L',
            'M',
            'N',
            'O',
            'P',
            'Q',
            'R',
            'S',
            'T',
            'U',
            'V',
            'W',
            'X',
            'Y',
            'Z',
            ';',
            '=',
            ',',
            '-',
            '.',
            '/',
            '`',
            '[',
            '\\',
            ']',
            '\'',
            0,
            0
        }
    };

static const char keypad_map[2][18] =
    {
        {
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            '\n',
            '*',
            '+',
            0,
            '-',
            '.',
            '/',
            '='
        },

        {
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            '\n',
            '*',
            '+',
            0,
            '-',
            0,
            '/',
            '='
        }
    };

inline Rml::Character GetCharacterCode(Rml::Input::KeyIdentifier key_identifier, int key_modifier_state)
{
    using Rml::Character;

    // Check if we have a keycode capable of generating characters on the main keyboard (ie, not on the numeric
    // keypad; that is dealt with below).
    if (key_identifier <= Rml::Input::KI_OEM_102)
    {
        // Get modifier states
        bool shift = (key_modifier_state & Rml::Input::KM_SHIFT) > 0;
        bool capslock = (key_modifier_state & Rml::Input::KM_CAPSLOCK) > 0;

        // Return character code based on identifier and modifiers
        if (shift && !capslock)
            return (Character)ascii_map[1][key_identifier];

        if (shift && capslock)
            return (Character)ascii_map[2][key_identifier];

        if (!shift && capslock)
            return (Character)ascii_map[3][key_identifier];

        return (Character)ascii_map[0][key_identifier];
    }

    // Check if we have a keycode from the numeric keypad.
    else if (key_identifier <= Rml::Input::KI_OEM_NEC_EQUAL)
    {
        if (key_modifier_state & Rml::Input::KM_NUMLOCK)
            return (Character)keypad_map[0][key_identifier - Rml::Input::KI_NUMPAD0];
        else
            return (Character)keypad_map[1][key_identifier - Rml::Input::KI_NUMPAD0];
    }

    else if (key_identifier == Rml::Input::KI_RETURN)
        return (Character)'\n';

    return Character::Null;
}
