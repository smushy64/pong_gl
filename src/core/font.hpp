#pragma once
#include "defines.hpp"
#include <map>

struct Glyph {
    i32 width, height;
    i32 xoff, yoff;
    i32 leftSideBearing;
    i32 advanceWidth;
    u8* bitmap;
};

struct Font {
    std::map<u8, Glyph> glyphs;
};

Font LoadFontFromBytes( u8* bytes );
void FreeFont(Font);
