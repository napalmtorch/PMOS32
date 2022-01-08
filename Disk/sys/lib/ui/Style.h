#pragma once
#include <sys/lib/stdint.h>
#include <sys/lib/stdarg.h>
#include <sys/lib/stddef.h>
#include <sys/lib/ctype.h>
#include <sys/lib/api/Util.h>
#include <sys/lib/gfx/Color.h>
#include <sys/lib/gfx/Font.h>

namespace pmgui
{
    enum class BorderStyle : uint8_t
    {
        None,
        Flat,
        Classic,
    };

    typedef struct
    {
        char        Name[32];
        int         ShadowSize;
        int         BorderSize;
        pmlib::Font Font;
        BorderStyle Border;
        uint32_t    Colors[8];
    } PACKED VisualStyle;

    namespace Styles
    {
        static const VisualStyle Default = 
        { 
            "Default", 0, 0, pmlib::Fonts::Serif8x8, BorderStyle::Classic,
            {
                (uint32_t)pmlib::Color::Silver,
                (uint32_t)pmlib::Color::Black,
                (uint32_t)pmlib::Color::White,
                (uint32_t)pmlib::Color::DimGray,
                (uint32_t)pmlib::Color::Black,
                (uint32_t)pmlib::Color::DarkGray,
                0, 0
            }
        };
    }
}