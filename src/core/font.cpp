#include "font.hpp"
#define STB_TRUETYPE_IMPLEMENTATION 1
#include "stb_truetype.h"

const f32 FONT_SIZE = 48.0f;

Font LoadFontFromBytes( u32 size, u8* bytes ) {
    stbtt_fontinfo font;
    stbtt_InitFont(
        &font, bytes,
        stbtt_GetFontOffsetForIndex( bytes, 0 )
    );

    Font result = {};
    for( u8 character = 0; character < 128; character++ ) {
        Glyph glyph = {};
        glyph.bitmap = stbtt_GetCodepointBitmap(
            &font, 0,
            stbtt_ScaleForPixelHeight(&font, FONT_SIZE),
            character,
            &glyph.width, &glyph.height,
            &glyph.xoff, &glyph.yoff
        );
        stbtt_GetCodepointHMetrics(&font, character, &glyph.advanceWidth, &glyph.leftSideBearing);

        result.glyphs.insert(std::pair<u8, Glyph>( character, glyph ));
    }
    return result;
}

void FreeFont(Font font) {
    for( u8 character = 0; character < 128; character++ ) {
        stbtt_FreeBitmap(
            font.glyphs[character].bitmap,
            nullptr
        );
    }
}
