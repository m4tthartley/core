
#include "../file.h"
#include <stdio.h>

int main() {
	m_arena mem;
	m_stack(&mem, 0, 0);
	m_reserve(&mem, GB(1), PAGE_SIZE);

	bitmap_t* bmp = f_load_bitmap(&mem, "font.bmp");
	u64 compressed_font[128] = {0};

	// bmp fontBmp = loadBmp("font.bmp");
	FOR (i, bmp->width*bmp->height) {
		if (bmp->data[i] == 0xFFFFFFFF) {
			bmp->data[i] = 0x0;
		}
	}
	FOR (c, 128) {
		int x = c%16 * (8);
		int y = c/(16) * 8;
		for(int p=0; p<64; ++p) {
			int* pixel = &bmp->data[(y+(p/8))*(8*16) + x+(p%8)];

			if (*pixel) {
				compressed_font[c] |= (1 << p);
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

	mem.stack = 0;

#define write(str) memcpy(m_push(&mem, s_len(str)), str, s_len(str));
	write("u64 DEFAULT_FONT[128] = {\n");
	FOR (i, 128) {
		FOR (a, 4) {
			char hex[32] = {0};
			sprintf(hex, "0x%x,", compressed_font[i+a]);
			write(hex);
		}
		i += 3;
		write("\n");
	}
	write("};\n");

	printf("outputting compressed font...\n");
	f_handle out = f_open("default_font.h");
	f_write(out, 0, mem.address, mem.stack);
	f_close(out);
}
