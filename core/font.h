//
//  font.h
//  Embedded Fonts
//
//  Created by Matt Hartley on 18/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#ifndef __CORE_FONT_HEADER__
#define __CORE_FONT_HEADER__


#include "core.h"


typedef struct {
	char a;
	char b;
	u8 value;
} font_kern_t;

typedef struct {
    u8 x;
    u8 y;
} font_kern_result_t;

typedef struct {
	u64 data[128];
	struct {
		u8 offsets[128];
		font_kern_t* pairs;
		int pair_count;
	} kerning;
} embedded_font_t;


#include "../tools/bin/font_data.h"


u8 font_get_kerning_pair(embedded_font_t* font, char a, char b) {
    FOR (i, font->kerning.pair_count) {
        font_kern_t* kern = font->kerning.pairs + i;
        if (kern->a == a && kern->b == b) {
            return kern->value;
        }
    }

    return 0;
}

font_kern_result_t font_get_kerning(embedded_font_t* font, char a, char b) {
    u8 xkern = font->kerning.offsets[a] >> 4;
    u8 ykern = font->kerning.offsets[a] & 0xF;

    if (b) {
        u8 kern_pair = font_get_kerning_pair(font, a, b);
        xkern += kern_pair;
    }

    return (font_kern_result_t){xkern, ykern};
}


#endif