//
//  Created by Matt Hartley on 18/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

#ifndef __CORE_FONT_HEADER__
#define __CORE_FONT_HEADER__


// #include "core.h"
#include <stdint.h>


typedef struct {
	char a;
	char b;
	uint8_t value;
} font_kern_t;

typedef struct {
    uint8_t x;
    uint8_t y;
} font_kern_result_t;

typedef struct {
	uint64_t data[128];
	struct {
		uint8_t offsets[128];
		font_kern_t* pairs;
		int pair_count;
	} kerning;
} embedded_font_t;

uint8_t font_get_kerning_pair(embedded_font_t* font, char a, char b);
font_kern_result_t font_get_kerning(embedded_font_t* font, char a, char b);


#endif


#ifdef CORE_IMPL
#	ifndef __CORE_FONT_HEADER_IMPL__
#	define __CORE_FONT_HEADER_IMPL__


#include "../tools/bin/font_data.h"


uint8_t font_get_kerning_pair(embedded_font_t* font, char a, char b) {
    for (int i=0; i<font->kerning.pair_count; ++i) {
        font_kern_t* kern = font->kerning.pairs + i;
        if (kern->a == a && kern->b == b) {
            return kern->value;
        }
    }

    return 0;
}

font_kern_result_t font_get_kerning(embedded_font_t* font, char a, char b) {
    uint8_t xkern = font->kerning.offsets[a] >> 4;
    uint8_t ykern = font->kerning.offsets[a] & 0xF;

    if (b) {
        uint8_t kern_pair = font_get_kerning_pair(font, a, b);
        xkern += kern_pair;
    }

    return (font_kern_result_t){xkern, ykern};
}


#	endif
#endif