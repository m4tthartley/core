//
//  font_generator.c
//  Core Font
//
//  Created by Matt Hartley on 29/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#define __SDL__
#define CORE_IMPL
#include <core/core.h>
#include <core/core.c>
#include <core/video.h>
#include <core/video.c>
#include <core/gfx.h>
#include <core/math.c>

typedef struct {
    u8 width;
    u8 yoffset;
} font_char_kern_t;

int main() {
	// m_arena mem;
	// m_stack(&mem, 0, 0);
	// m_reserve(&mem, GB(1), PAGE_SIZE);

    allocator_t arena = create_allocator(NULL, MB(100));
    u8 buffer[MB(1)] = {0};
    memstack_t stack = create_stack(buffer, MB(10));

	bitmap_t* bmp = load_bitmap_file(&arena, "font_v5.bmp");
	
    font_char_kern_t kerning[128] = {0};
    u64 compressed_font[128] = {0};

	// bmp fontBmp = loadBmp("font.bmp");
	// FOR (i, bmp->width*bmp->height) {
	// 	if (bmp->data[i] == 0xFFFFFFFF) {
	// 		bmp->data[i] = 0x0;
	// 	}
	// }
	FOR (c, 128) {
		int x = c%16 * (8);
		int y = c/(16) * 8;
		for(int p=0; p<64; ++p) {
			u32* pixel = &bmp->data[(y+(p/8*(8*16))) + x+(p%8)];

			if (*pixel /*== 0xFFFFFFFF*/) {
				compressed_font[c] |= (1L << p);
			} else {
				
			}

			// byte horizontal = game->font.kerning[c] & 0x0f;
			// byte verticle = (game->font.kerning[c] & 0xf0)>>4;
			// if(*pixel == 0xFFFF0000) {
			// 	game->font.kerning[c] = (verticle<<4) | ++horizontal;
			// 	*pixel = 0;
			// }
			// if(*pixel == 0xFF00FF00) {
			// 	game->font.kerning[c] = (++verticle<<4) | horizontal;
			// 	*pixel = 0;
			// }
			// if(*pixel == 0xFF000000) {
			// 	*pixel = 0xFFFFFFFF;
			// }
		}
	}

	stack.stack = 0;
    

#define write(str) memcpy(push_memory(&stack, str_len(str)), str, str_len(str));
	write("u64 DEFAULT_FONT[128] = {\n");
	FOR (i, 128) {
		FOR (a, 4) {
			char hex[32] = {0};
			sprintf(hex, "\t0x%016llx,", compressed_font[i+a]);
			write(hex);
		}
		i += 3;
		write("\n");
	}
	write("};\n");

	printf("outputting compressed font...\n");
	file_t out = file_open("default_font.h");
	file_write(out, 0, stack.address, stack.stack);
	file_close(out);
}
