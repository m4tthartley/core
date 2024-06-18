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
	// write(
	// 	"\n"
	// 	"struct font_kern_t {\n"
	// 	"\tchar a;\n"
	// 	"\tchar b;\n"
	// 	"\tu8 amount;\n"
	// 	"};\n"
	// 	"\n"
	// 	"typedef struct {\n"
	// 	"\tu64 data[128];\n"

	// 	"\tstruct {\n"
	// 	"\t\tu8 offsets[128];\n"
	// 	"\t\tstruct font_kern_t* kerns;\n"
	// 	"\t\tint kern_count;\n"
	// 	"\t} kerning;\n"
		
	// 	"} embedded_font_t;\n"
	// 	"\n"
	// );

	font_kern_t kerns[] = {
		{ '\'', 'J', 2 },
		{ '\'', 'a', 1 },
		{ '\'', 'c', 1 },
		{ '\'', 'd', 1 },
		{ '\'', 'e', 1 },
		{ '\'', 'f', 1 },
		{ '\'', 'g', 1 },
		{ '\'', 'j', 1 },

		{ '\'', 'n', 1 },
		{ '\'', 'o', 1 },
		{ '\'', 'q', 1 },
		{ '\'', 'r', 1 },
		{ '\'', 's', 1 },

		{ 'A', 'V', 0 },
		{ 'A', 'j', 2 },
		{ 'B', 'f', 1 },
		{ 'B', 'j', 2 },
		{ 'C', 'f', 1 },
		{ 'C', 'j', 2 },
		{ 'C', 's', 1 },
		{ 'D', 'f', 1 },
		{ 'D', 'j', 2 },
		{ 'E', 'j', 2 },
		{ 'E', 's', 1 },
		{ 'E', 'v', 1 },
		{ 'F', 'J', 2 },
		{ 'F', 'a', 1 },
		{ 'F', 'f', 1 },
		{ 'F', 'j', 2 },
		{ 'G', '\'', 1 },
		{ 'G', 'T', 1 },
		{ 'G', 'f', 1 },
		{ 'G', 'j', 2 },
		{ 'H', 'j', 2 },
		{ 'I', 'j', 2 },
		{ 'I', 's', 1 },
		{ 'I', 'v', 1 },
		{ 'J', 'f', 1 },
		{ 'J', 'j', 2 },
		{ 'K', 'j', 2 },
		{ 'L', '\'', 1 },
		{ 'L', 'T', 2 },
		{ 'L', 'V', 1 },
		{ 'L', 'Y', 2 },
		{ 'L', 'j', 2 },
		{ 'L', 's', 1 },
		{ 'L', 'v', 1 },
		{ 'M', 'j', 2 },
		{ 'N', 'j', 2 },
		{ 'O', 'f', 1 },
		{ 'O', 'j', 2 },
		{ 'P', 'J', 3 },
		{ 'P', 'a', 1 },
		{ 'P', 'f', 1 },
		{ 'P', 'j', 2 },
		{ 'Q', 'j', 2 },
		{ 'R', 'j', 2 },
		{ 'S', 'f', 1 },
		{ 'S', 'j', 2 },
		{ 'T', 'J', 2 },
		{ 'T', 'a', 2 },
		{ 'T', 'c', 2 },
		{ 'T', 'd', 2 },
		{ 'T', 'e', 2 },
		{ 'T', 'f', 1 },
		{ 'T', 'g', 2 },
		{ 'T', 'i', 1 },
		{ 'T', 'j', 2 },
		{ 'T', 'm', 2 },
		{ 'T', 'n', 2 },
		{ 'T', 'o', 2 },
		{ 'T', 'p', 2 },
		{ 'T', 'q', 2 },
		{ 'T', 'r', 2 },
		{ 'T', 's', 2 },
		{ 'T', 'u', 2 },
		{ 'T', 'v', 2 },
		{ 'T', 'w', 2 },
		{ 'T', 'x', 2 },
		{ 'T', 'y', 2 },
		{ 'T', 'z', 2 },
		{ 'U', 'f', 1 },
		{ 'U', 'j', 2 },
		{ 'V', 'f', 1 },
		{ 'V', 'j', 2 },
		{ 'W', 'j', 2 },
		{ 'X', 'j', 2 },
		{ 'X', 's', 1 },
		{ 'Y', 'J', 2 },
		{ 'Y', 'a', 1 },
		{ 'Y', 'c', 1 },
		{ 'Y', 'd', 1 },
		{ 'Y', 'e', 1 },
		{ 'Y', 'f', 1 },
		{ 'Y', 'g', 1 },
		{ 'Y', 'j', 2 },
		{ 'Y', 'n', 1 },
		{ 'Y', 'o', 1 },
		{ 'Y', 'q', 1 },
		{ 'Y', 'r', 1 },
		{ 'Y', 's', 1 },
		{ 'Z', 'j', 2 },
		{ 'Z', 's', 1 },

		{ 'a', '\'', 1 },
		{ 'a', 'T', 2 },
		{ 'a', 'Y', 1 },
		{ 'a', 'j', 2 },
		{ 'b', '\'', 1 },
		{ 'b', 'T', 2 },
		{ 'b', 'Y', 1 },
		{ 'b', 'f', 1 },
		{ 'b', 'j', 2 },
		{ 'c', 'T', 2 },
		{ 'c', 'Y', 1 },
		{ 'c', 'f', 1 },
		{ 'c', 'j', 2 },
		{ 'd', 'f', 1 },
		{ 'd', 'j', 2 },
		{ 'e', 'T', 2 },
		{ 'e', 'Y', 1 },
		{ 'e', 'j', 2 },
		{ 'f', 'J', 2 },
		{ 'f', 'a', 1 },
		{ 'f', 'c', 1 },
		{ 'f', 'd', 1 },
		{ 'f', 'e', 1 },
		{ 'f', 'f', 1 },
		{ 'f', 'g', 1 },
		{ 'f', 'j', 2 },
		{ 'f', 'n', 1 },
		{ 'f', 'o', 1 },
		{ 'f', 'q', 1 },
		{ 'f', 'r', 1 },
		{ 'f', 's', 1 },
		{ 'g', 'T', 2 },
		{ 'g', 'Y', 1 },
		{ 'h', 'T', 2 },
		{ 'h', 'Y', 1 },
		{ 'h', 'j', 2 },
		{ 'i', 'j', 2 },
		{ 'k', 'j', 2 },
		{ 'k', 's', 1 },
		{ 'm', 'T', 2 },
		{ 'm', 'Y', 1 },
		{ 'm', 'j', 2 },
		{ 'n', '\'', 2 },
		{ 'n', 'T', 2 },
		{ 'n', 'Y', 2 },
		{ 'n', 'j', 2 },
		{ 'o', 'T', 2 },
		{ 'o', 'Y', 1 },
		{ 'o', 'f', 1 },
		{ 'o', 'j', 2 },
		{ 'p', 'T', 2 },
		{ 'p', 'Y', 1 },
		{ 'p', 'f', 1 },
		{ 'p', 'j', 2 },
		{ 'q', 'T', 2 },
		{ 'r', 'I', 1 },
		{ 'r', 'J', 3 },
		{ 'r', 'T', 2 },
		{ 'r', 'Z', 1 },
		{ 'r', 'a', 1 },
		{ 'r', 'f', 1 },
		{ 'r', 'j', 2 },
		{ 's', 'T', 2 },
		{ 's', 'Y', 1 },
		{ 's', 'f', 1 },
		{ 's', 'j', 2 },
		{ 's', 'q', 0 },
		{ 's', 's', 1 },
		{ 't', '\'', 1 },
		{ 't', 'T', 2 },
		{ 't', 'Y', 1 },
		{ 't', 'j', 2 },
		{ 'u', 'T', 2 },
		{ 'u', 'f', 1 },
		{ 'u', 'j', 2 },
		{ 'v', 'I', 1 },
		{ 'v', 'T', 2 },
		{ 'v', 'f', 1 },
		{ 'v', 'j', 2 },
		{ 'w', 'T', 2 },
		{ 'w', 'j', 2 },
		{ 'x', 'T', 2 },
		{ 'x', 'j', 2 },
		{ 'y', 'T', 2 },
		{ 'z', 'T', 2 },
		{ 'z', 'j', 2 },
	};

	write("\n");
	write("font_kern_t FONT_DEFAULT_KERNS[] = {\n");
	FOR (i, array_size(kerns)) {
		write("\t");
		FOR (a, umin(4, array_size(kerns)-i)) {
			font_kern_t kern = kerns[i+a];
			char line[32] = {0};
			sprintf(line, "{ '%s%c', '%s%c', %u },", kern.a=='\''?"\\":"", kern.a, kern.b=='\''?"\\":"", kern.b, (u32)kern.value);
			write(line);
		}
		write("\n");
		i += 3;
	}
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
	write("\t\t.offsets = {\n");
	FOR (i, 128) {
		write("\t\t\t");
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
	char line[32];
	sprintf(line, "\t\t.kern_count = 0x%x,\n", (u32)array_size(kerns));
	write(line);
	write("\t},\n");

	write("};\n");

	printf("outputting compressed font...\n");
	file_t out = file_create("../bin/font_data.h");
	file_write(out, 0, stack.address, stack.stack);
	file_truncate(out, stack.stack);
	file_close(out);
}
