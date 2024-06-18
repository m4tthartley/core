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
	u64 data[128];
	struct {
		u8 offsets[128];
		font_kern_t* kerns;
		int kern_count;
	} kerning;
} embedded_font_t;


#include "../tools/bin/font_data.h"


u8 font_get_kerning(embedded_font_t* font, char a, char b) {
    FOR (i, font->kerning.kern_count) {
        font_kern_t* kern = font->kerning.kerns + i;
        if (kern->a == a && kern->b == b) {
            return kern->value;
        }
    }

    return 0;
}


#endif