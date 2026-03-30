#pragma once

#include <RmlUi/Core/Input.h>
#include "keydefs.h"
#include "vgui/KeyCode.h"

// Keycode converter helper.
inline const Rml::Input::KeyIdentifier ButtonToRocketKey( int button )
{
    using namespace Rml::Input;

    switch( button )
    {
    // case KEY_0:
    //     return KI_0;
    // case KEY_1:
    //     return KI_1;
    // case KEY_2:
    //     return KI_2;
    // case KEY_3:
    //     return KI_3;
    // case KEY_4:
    //     return KI_4;
    // case KEY_5:
    //     return KI_5;
    // case KEY_6:
    //     return KI_6;
    // case KEY_7:
    //     return KI_7;
    // case KEY_8:
    //     return KI_8;
    // case KEY_9:
    //     return KI_9;
    // case KEY_A:
    //     return KI_A;
    // case KEY_B:
    //     return KI_B;
    // case KEY_C:
    //     return KI_C;
    // case KEY_D:
    //     return KI_D;
    // case KEY_E:
    //     return KI_E;
    // case KEY_F:
    //     return KI_F;
    // case KEY_G:
    //     return KI_G;
    // case KEY_H:
    //     return KI_H;
    // case KEY_I:
    //     return KI_I;
    // case KEY_J:
    //     return KI_J;
    // case KEY_K:
    //     return KI_K;
    // case KEY_L:
    //     return KI_L;
    // case KEY_M:
    //     return KI_M;
    // case KEY_N:
    //     return KI_N;
    // case KEY_O:
    //     return KI_O;
    // case KEY_P:
    //     return KI_P;
    // case KEY_Q:
    //     return KI_Q;
    // case KEY_R:
    //     return KI_R;
    // case KEY_S:
    //     return KI_S;
    // case KEY_T:
    //     return KI_T;
    // case KEY_U:
    //     return KI_U;
    // case KEY_V:
    //     return KI_V;
    // case KEY_W:
    //     return KI_W;
    // case KEY_X:
    //     return KI_X;
    // case KEY_Y:
    //     return KI_Y;
    // case KEY_Z:
    //     return KI_Z;
    // case K_KP_0:
    //     return KI_NUMPAD0;
    // case K_KP_1:
    //     return KI_NUMPAD1;
    // case K_KP_2:
    //     return KI_NUMPAD2;
    // case K_KP_3:
    //     return KI_NUMPAD3;
    // case K_KP_4:
    //     return KI_NUMPAD4;
    // case K_KP_5:
    //     return KI_NUMPAD5;
    // case K_KP_6:
    //     return KI_NUMPAD6;
    // case K_KP_7:
    //     return KI_NUMPAD7;
    // case K_KP_8:
    //     return KI_NUMPAD8;
    // case K_KP_9:
    //     return KI_NUMPAD9;
    // case K_KP_DIVIDE:
    //     return KI_DIVIDE;
    // case K_KP_MULTIPLY:
    //     return KI_MULTIPLY;
    case K_KP_MINUS:
        return KI_SUBTRACT;
    case K_KP_PLUS:
        return KI_ADD;
    case K_KP_ENTER:
        return KI_NUMPADENTER;
    // case K_KP_DECIMAL:
    //     return KI_DECIMAL;
    // case KEY_LBRACKET:
    //     return KI_OEM_4;
    // case KEY_RBRACKET:
    //     return KI_OEM_6;
    // case KEY_SEMICOLON:
    //     return KI_OEM_1;
    // case KEY_APOSTROPHE:
    //     return KI_OEM_7;
    // case KEY_BACKQUOTE:
    //     return KI_OEM_3;
    // case KEY_COMMA:
    //     return KI_OEM_COMMA;
    // case KEY_PERIOD:
    //     return KI_OEM_PERIOD;
    // case KEY_SLASH:
    //     return KI_OEM_2;
    // case KEY_BACKSLASH:
    //     return KI_OEM_5;
    // case KEY_MINUS:
    //     return KI_OEM_MINUS;
    // case KEY_EQUAL:
    //     return KI_OEM_PLUS;
    case K_ENTER:
        return KI_RETURN;
    case K_SPACE:
        return KI_SPACE;
    case K_BACKSPACE:
        return KI_BACK;
    case K_TAB:
        return KI_TAB;
    //case KEY_CAPSLOCK:
    //    return KI_
    //case KEY_NUMLOCK:
    //    return KI_
    case K_ESCAPE:
        return KI_ESCAPE;
    //case KEY_SCROLLLOCK:
    //    return KI_
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
    //case KEY_LSHIFT:
    //    return KI
    //case KEY_RSHIFT:
    //    return KI_
    //case KEY_LALT:
    //    return KI_
    //case KEY_RALT:
    //    return KI_
    //case KEY_LCONTROL:
    //    return KI_
    //case KEY_RCONTROL:
    //    return KI_
    //case KEY_LWIN:
    //    return KI_
    //case KEY_RWIN:
    //    return KI_
    //case KEY_APP:
    //    return KI_
    case K_UPARROW:
        return KI_UP;
    case K_LEFTARROW:
        return KI_LEFT;
    case K_DOWNARROW:
        return KI_DOWN;
    case K_RIGHTARROW:
        return KI_RIGHT;
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
        //case KEY_CAPSLOCKTOGGLE:
        //    return KI_
        //case KEY_NUMLOCKTOGGLE:
        //    return KI_
        //case KEY_SCROLLLOCKTOGGLE:
        //    return KI_

    default:
        return KI_UNKNOWN;
    }
    return KI_UNKNOWN;
}

/**
    This map contains 4 different mappings from key identifiers to character codes. Each entry represents a different
    combination of shift and capslock state.
 */

char ascii_map[4][51] =
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

char keypad_map[2][18] =
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
