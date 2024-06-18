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

#   define print_bin64(num)\
		for(int bit=0; bit<64; ++bit) printf(num & ((u64)1 << bit) ? "1" : "0");

	// bmp fontBmp = loadBmp("font.bmp");
	// FOR (i, bmp->width*bmp->height) {
	// 	if (bmp->data[i] == 0xFFFFFFFF) {
	// 		bmp->data[i] = 0x0;
	// 	}
	// }
	FOR (c, 128) {
		// printf("c%i\n", c);
		int charx = c%16 * 8;
		int chary = c/(16) * 8;
		for(int p=0; p<64; ++p) {
			int x = charx + (p%8);
			int y = chary + (p/8);
			u32 pixel = bmp->data[y * bmp->width + x];

			// printf("0x%08x \n", pixel);
			u64 pixel_bit = (u64)1 << (u64)p;
			if (pixel == 0xFFFFFFFF) {
				compressed_font[c] |= (pixel_bit);
				// printf("[%llu] ", pixel_bit);
			} else {
				// printf("%llu ", pixel_bit);
			}

			// RED
			if (pixel == 0xFFFF0000) {
				++kerning[c].width;
			}
			// GREEN
			if (pixel == 0xFF00FF00) {
				++kerning[c].yoffset;
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
		// print_bin64(compressed_font[c]);
		// printf("\n\n");

		// if ((c/16)%2 == 0 ? (c%2 == 0) : (c%2 == 1)) {
		//     compressed_font[c] = 0xFFFFFFFFFFFFFFFF;
		// } else {
		//     compressed_font[c] = 0xAA55AA55AA55AA55;
		// }
	}

	stack.stack = 0;
	

#define write(str) memcpy(push_memory(&stack, str_len(str)), str, str_len(str));
	write(
		"\n"
		"struct font_kern_t {\n"
		"\tchar a;\n"
		"\tchar b;\n"
		"\tu8 amount;\n"
		"};\n"
		"\n"
		"typedef struct {\n"
		"\tu64 data[128];\n"

		"\tstruct {\n"
		"\t\tu8 offsets[128];\n"
		"\t\tstruct font_kern_t* kerns;\n"
		"\t\tint kern_count;\n"
		"\t} kerning;\n"
		
		"} embedded_font_t;\n"
		"\n"
	);

	write("struct font_kern_t FONT_DEFAULT_KERNS[] = {\n");
	write("\t{ 'f', 'g', 1 }\n");
	write("};\n");

	write("embedded_font_t FONT_DEFAULT = {\n");

	write("\t.data = {\n");
	FOR (i, 128) {
		write("\t\t");
		FOR (a, 4) {
#ifdef DEBUG
			char hex[65] = {0};
			// sprintf(hex, "0x%016llx,", compressed_font[i+a]);
			for(int bit=0; bit<64; ++bit)
				hex[bit] = compressed_font[i+a] & ((u64)1 << bit) ? '1' : '0';
			char character[128] = {0};
			sprintf(character, "/*%i*/0b%s,", i+a, hex);
			write(character);
#else
			char hex[32] = {0};
			sprintf(hex, "0x%016llx,", compressed_font[i+a]);
			write(hex);
#endif
		}
		i += 3;
		write("\n");
	}
	write("\t},\n");

	write("\t.kerning = {\n");
	write("\t\t\t.offsets = {\n");
	FOR (i, 128) {
		write("\t\t");
		FOR (a, 16) {
			char hex[32] = {0};
			u32 kern = kerning[i+a].width << 4 | kerning[i+a].yoffset;
			sprintf(hex, "0x%02x,", kern);
			write(hex);
		}
		i += 15;
		write("\n");
	}
	write("\t\t},\n");
	write("\t\t.kerns = FONT_DEFAULT_KERNS,\n");
	write("\t},\n");

	write("};\n");

	printf("outputting compressed font...\n");
	file_t out = file_open("default_font.h");
	file_write(out, 0, stack.address, stack.stack);
	file_truncate(out, stack.stack);
	file_close(out);
}
