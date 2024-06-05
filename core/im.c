//
//  im.c
//  Core
//
//  Created by Matt Hartley on 16/11/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include "video.h"
#include "math.h"
#include "im.h"
#include "../font/default_font.h"

gfx_texture_t* _gfx_active_texture = NULL;
// vec2_t _gfx_coord_system = {1.0f, 1.0f};
v2 _gfx_ortho_space = {2.0f, 2.0f};
v2 _gfx_ortho_res_scale = {1.0f, 1.0f};

void APIENTRY _opengl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char* message, const void *userParam) {
	print("[id:%i] %s\n", id, message);
}

void start_opengl_debug() {
	glDebugMessageCallback(_opengl_debug_callback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

bitmap_t* load_bitmap_file(allocator_t* allocator, char* filename) {
	// TODO replace with platform functions
	FILE* fontFile;
	long fileSize;
	void* fontData;
	bmp_header_t* header;
	u32* palette;
	char* data;
	int rowSize;

	fontFile = fopen(filename, "r"); // todo: this stuff crashes when file not found
	if(!fontFile) {
		print_error("Unable to open file: %s", filename);
		return NULL;
	}
	fseek(fontFile, 0, SEEK_END);
	fileSize = ftell(fontFile);
	fontData = malloc(fileSize);
	rewind(fontFile);
	fread(fontData, 1, fileSize, fontFile);
	fclose(fontFile);
	
	header = (bmp_header_t*)fontData;
	palette = (u32*)((char*)fontData+14+header->headerSize);
	data = (char*)fontData+header->offset;
	rowSize = ((header->colorDepth*header->bitmapWidth+31) / 32) * 4;

	// Possibly check whether to alloc or push
	bitmap_t* result = alloc_memory_in(allocator, sizeof(bitmap_t) + sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
	result->size = header->size;
	result->width = header->bitmapWidth;
	result->height = header->bitmapHeight;
	
	u32* image = (u32*)(result + 1);
	// image = (u32*)malloc(sizeof(u32)*header->bitmapWidth*header->bitmapHeight);
	//{for(int w=0; w<header.bitmapHeight}
	{
		int row;
		int pixel;
		for(row=0; row<header->bitmapHeight; ++row) {
			int bitIndex=0;
			//printf("row %i \n", row);
// 			if(row==255) {
// 				DebugBreak();
// 			}
			for(pixel=0; pixel<header->bitmapWidth; ++pixel) {//while((bitIndex/8) < rowSize) {
				u32* chunk = (u32*)((char*)fontData+header->offset+(row*rowSize)+(bitIndex/8));
				u32 pi = *chunk;
				if(header->colorDepth<8) {
					pi >>= (header->colorDepth-(bitIndex%8));
				}
				pi &= (((i64)1<<header->colorDepth)-1);
				if(header->colorDepth>8) {
					image[row*header->bitmapWidth+pixel] = pi;
				} else {
					image[row*header->bitmapWidth+pixel] = palette[pi];
				}

				image[row*header->bitmapWidth+pixel] |= 0xFF << 24;

				if(image[row*header->bitmapWidth+pixel]==0xFF000000 ||
				   image[row*header->bitmapWidth+pixel]==0xFFFF00FF) {
					image[row*header->bitmapWidth+pixel] = 0;
				}

				bitIndex += header->colorDepth;
			}
		}
	}

	free(fontData);
	
	// result.data = image;
	// result.header = header;
	return result;
}

bitmap_t* load_font_file(allocator_t* allocator, char*filename) {
	bitmap_t* bitmap = load_bitmap_file(allocator, filename);
	// if (bitmap) {
	// 	u32* pixels = (u32*)(bitmap + 1);
	// 	FOR (i, bitmap->width * bitmap->height) {
	// 		u32 pixel = pixels[i];
	// 		if (pixel != 0xFFFF0000) {
	// 			pixels[i] = 0;
	// 		} else {
	// 			pixels[i] = 0xFFFFFFFF;
	// 		}
	// 	}
	// }

	return bitmap;
}

gfx_texture_t gfx_create_null_texture(int width, int height) {
	int block_size = 2;
	u32* data = malloc(sizeof(u32) * width * height);
	for(int y=0; y<height; ++y) for(int x=0; x<width; ++x) {
		if(((y/block_size)&1) ? (x / block_size) & 1 : !((x / block_size) & 1)) {
			data[y*width+x] = (255<<0) | (255<<8) | (255<<16) | (255<<24);
		} else {
			data[y*width+x] = (0<<0) | (0) | (0<<16) | (255<<24);
		}
	}
	GLuint handle;
	glGenTextures(1, &handle);
	glBindTexture(GL_TEXTURE_2D, handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	free(data);
	gfx_texture_t result = {
		handle,
		width,
		height
	};
	return result;
}

gfx_texture_t gfx_create_texture(bitmap_t* bitmap) {
	if (bitmap) {
		gfx_texture_t result;
		result.width = bitmap->width;
		result.height = bitmap->height;
		glGenTextures(1, &result.handle);
		glBindTexture(GL_TEXTURE_2D, result.handle);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap->width, bitmap->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap->data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		return result;
	} else {
		return gfx_create_null_texture(256, 256);
	}
}

void gfx_update_texture(gfx_texture_t* texture, bitmap_t* bitmap) {
	glBindTexture(GL_TEXTURE_2D, texture->handle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmap->data);
}

void gfx_texture(gfx_texture_t* texture) {
	if (texture) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture->handle);
	} else {
		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	_gfx_active_texture = texture;
}

void gfx_clear(vec4_t color) {
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// void gfx_color(vec4_t color) {
// 	glColor4f(color.r, color.g, color.b, color.a);
// }

void gfx_ortho_projection(window_t* window, f32 left, f32 right, f32 bottom, f32 top) {
	// _gfx_coord_system = vec2(1.0f / width, 1.0f / height);
	_gfx_ortho_space = vec2(right-left, top-bottom);
	_gfx_ortho_res_scale = vec2((right-left) / (f32)window->width, (top-bottom) / (f32)window->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(left, right, bottom, top, -10, 10);
	glMatrixMode(GL_MODELVIEW);
}

void gfx_ortho_projection_centered(window_t* window, f32 width, f32 height) {
	gfx_ortho_projection(window, -width/2.0f, width/2.0f, -height/2.0f, height/2.0f);
}

void gfx_color(v4 color) {
	glColor4f(color.r, color.g, color.b, color.a);
}

void gfx_point(vec2_t pos) {
	glBegin(GL_POINTS);
	glVertex2f(pos.x, pos.y);
	glEnd();
}

void gfx_quad(vec2_t pos, vec2_t size) {
	// glDisable(GL_TEXTURE_2D);

	vec2_t s = {
		size.x/2.0f,
		size.y/2.0f,
	};
	glBegin(GL_QUADS);
	glVertex2f(pos.x-s.x, pos.y-s.y);
	glVertex2f(pos.x+s.x, pos.y-s.y);
	glVertex2f(pos.x+s.x, pos.y+s.y);
	glVertex2f(pos.x-s.x, pos.y+s.y);
	glEnd();
}

void gfx_sprite(window_t* window, vec2_t pos, int px, int py, int pxs, int pys, float scale) {
	// vec2_t percent_of_screen = vec2(1.0f/((f32)window->width/pxs), 1.0f/((f32)window->height/pys));
	// vec2_t s = {
	// 	(percent_of_screen.x/*/_gfx_coord_system.x*/) * (f32)scale,
	// 	(percent_of_screen.y/*/_gfx_coord_system.y*/) * (f32)scale,
	// };
	vec2_t s = mul2(mul2f(vec2(pxs/2, pys/2), scale), _gfx_ortho_res_scale);
	gfx_texture_t* t = _gfx_active_texture;
	glBegin(GL_QUADS);
	glTexCoord2f((f32)px / (f32)t->width, ((f32)py) / (f32)t->height);
	glVertex2f(pos.x-s.x, pos.y-s.y);

	glTexCoord2f(((f32)px+pxs) / (f32)t->width, ((f32)py) / (f32)t->height);
	glVertex2f(pos.x+s.x, pos.y-s.y);

	glTexCoord2f(((f32)px+pxs) / (f32)t->width, ((f32)py+pys) / (f32)t->height);
	glVertex2f(pos.x+s.x, pos.y+s.y);

	glTexCoord2f((f32)px / (f32)t->width, ((f32)py+pys) / (f32)t->height);
	glVertex2f(pos.x-s.x, pos.y+s.y);
	glEnd();

}

void gfx_sprite_tile(window_t* window, gfx_sprite_t* sprite, vec2_t pos, int tile) {
	gfx_texture(&sprite->texture);
	int tiles_per_row = sprite->texture.width / sprite->tile_size;
	int tiles_per_column = sprite->texture.height / sprite->tile_size;
	tile %= (tiles_per_row * tiles_per_column);
	gfx_sprite(window, pos,
			(tile%tiles_per_row)*sprite->tile_size,
			(tile/tiles_per_row)*sprite->tile_size,
			sprite->tile_size, sprite->tile_size, sprite->scale);
}

void gfx_circle(vec2_t pos, f32 size, int segments) {
	// glDisable(GL_TEXTURE_2D);

	vec2_t s = {
		size/2.0f,
		size/2.0f,
	};
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(pos.x, pos.y);
	for(int i=0; i<segments; ++i) {
		glVertex2f(
				pos.x+sinf(PI2*((f32)i/segments))*s.x,
				pos.y+cosf(PI2*((f32)i/segments))*s.y);
	}
	glVertex2f(
			pos.x+sinf(PI2)*s.x,
			pos.y+cosf(PI2)*s.y);
	glEnd();
}

void gfx_line_circle(vec2_t pos, f32 size, int segments) {
	// glDisable(GL_TEXTURE_2D);

	vec2_t s = {
		size/2.0f,
		size/2.0f,
	};
	glBegin(GL_LINE_STRIP);
	// glVertex2f(pos.x, pos.y);
	for(int i=0; i<segments; ++i) {
		glVertex2f(
				pos.x+sinf(PI2*((f32)i/segments))*s.x,
				pos.y+cosf(PI2*((f32)i/segments))*s.y);
	}
	glVertex2f(
			pos.x+sinf(PI2)*s.x,
			pos.y+cosf(PI2)*s.y);
	glEnd();
}

void gfx_line(vec2_t start, vec2_t end) {
	// glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glVertex2f(start.x, start.y);
	glVertex2f(end.x, end.y);
	glEnd();
}

void gfx_text(window_t* window, vec2_t pos, float scale, char* str, ...) {
	if (!_gfx_active_texture) {
		print_error("gfx_text: No active texture");
		return;
	}

	char b[1024];
	va_list args;
	va_start(args, str);
	// int length = vsnprintf(0, 0, layout.text, args) + 1;
	// char* buffer = pushMemory(&ui->transient, length);
	vsnprintf(b, sizeof(b), str, args);

	// vec2_t pixel_size = vec2(1.0f/(window->width/8), 1.0f/(window->height/8));
	// vec2_t s = {
	// 	(pixel_size.x)*2.0f * (f32)scale,
	// 	(pixel_size.y)*2.0f * (f32)scale,
	// };
	
	// vec2_t percent_of_screen = vec2(1.0f/((f32)window->width/8), 1.0f/((f32)window->height/8));
	// vec2_t s = {
	// 	(percent_of_screen.x/*/_gfx_coord_system.x*/) * (f32)scale,
	// 	(percent_of_screen.y/*/_gfx_coord_system.y*/) * (f32)scale,
	// };

	gfx_texture_t* t = _gfx_active_texture;
	int chars_per_row = t->width / 8;
	int chars_per_col = t->height / 8;
	
	// float charSize = 1.0f*0.05f;
	char* buffer = b;
	for (int i=0; *buffer; ++i,++buffer) {
		if(*buffer != '\n') {
			// vec2_t uv = vec2((float)(*buffer%chars_per_row) / (float)chars_per_row,
			// 		(float)(*buffer/chars_per_row) / (float)chars_per_col);
			// vec2_t uvt = vec2(1.0f/(float)chars_per_row, 1.0f/(float)chars_per_col);
			// vec2_t charPos = add2(pos, vec2(i * s.x, 0));
			// glBegin(GL_QUADS);
			// glTexCoord2f(uv.x,       uv.y+uvt.y); glVertex2f(charPos.x,            charPos.y+s.y);
			// glTexCoord2f(uv.x+uvt.x, uv.y+uvt.y); glVertex2f(charPos.x+(s.x), charPos.y+s.y);
			// glTexCoord2f(uv.x+uvt.x, uv.y);       glVertex2f(charPos.x+(s.x), charPos.y);
			// glTexCoord2f(uv.x,       uv.y);       glVertex2f(charPos.x,            charPos.y);
			// glEnd();

			v2 char_pos = add2(pos, mul2(vec2((float)i*8.0f*scale, 0), _gfx_ortho_res_scale));
			i2 pixel_offset = int2(*buffer%chars_per_row * 8, *buffer/chars_per_row * 8);
			gfx_sprite(window, char_pos, pixel_offset.x, pixel_offset.y, 8, 8, scale);
		}
	}
}
