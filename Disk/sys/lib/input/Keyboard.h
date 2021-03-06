#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>

namespace pmlib
{
    enum class Key : uint8_t
    {
        Null,
        Escape,
        D1,
        D2,
        D3,
        D4,
        D5,
        D6,
        D7,
        D8,
        D9,
        D0,
        Minus,
        Equal,
        Backspace,
        Tab,
        Q,
        W,
        E,
        R,
        T,
        Y,
        U,
        I,
        O,
        P,
        LeftBracket,
        RightBracket,
        Enter,
        LeftCtrl,
        A,
        S,
        D,
        F,
        G,
        H,
        J,
        K,
        L,
        Colon,
        Quote,
        Backtick,
        LeftShift,
        Backslash,
        Z,
        X,
        C,
        V,
        B,
        N,
        M,
        Comma,
        Period,
        Slash,
        RightShift,
        Multiply,
        LeftAlt,
        Space,
        CapsLock,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        NumLock,
        ScrollLock,
        Home,
        UpArrow,
        PageUp,
        KeypadMinus,
        LeftArrow,
        Keypad5,
        RightArrow,
        Plus,
        End,
        DownArrow,
        PageDown,
        Delete,
        F11 = 0x57,
        F12,
    };

    bool KeyboardDown(Key key);
    bool KeyboardUp(Key key);
}