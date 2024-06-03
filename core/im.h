//
//  im.h
//  Core
//
//  Created by Matt Hartley on 18/09/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

// #include <windows.h>
// #include <gl/gl.h>
// #include <file.h>

// typedef GLuint glh_t;

#ifndef __CORE_IM_HEADER__
#define __CORE_IM_HEADER__


#include "core.h"
#include "math.h"

#pragma pack(push, 1)
typedef struct {
	char header[2];
	u32 size;
	u16 reserved1;
	u16 reserved2;
	u32 offset;
	
	// Windows BITMAPINFOHEADER
	u32 headerSize;
	i32 bitmapWidth;
	i32 bitmapHeight;
	u16 colorPlanes;
	u16 colorDepth;
	u32 compression;
	u32 imageSize;
	i32 hres;
	i32 vres;
	u32 paletteSize;
	u32 importantColors;
} bmp_header_t;
#pragma pack(pop)

typedef struct {
	u32* data;
	bmp_header_t* header;
} bmp_file_t;

typedef struct {
	u32 size;
	u32 width;
	u32 height;
	u32 data[];
} bitmap_t;

typedef struct {
	GLuint handle;
	f32 width;
	f32 height;
} gfx_texture_t;

// typedef struct {
// 	gfx_texture_t texture;
// 	vec2_t uv;
// 	vec2_t uv2;
// } gfx_sprite_t;

typedef struct {
	gfx_texture_t texture;
	int tile_size;
	int scale;
} gfx_sprite_t;

bitmap_t* load_bitmap_file(allocator_t* allocator, char* filename);
bitmap_t* load_font_file(allocator_t* allocator, char*filename);

gfx_texture_t gfx_create_null_texture(int width, int height);
gfx_texture_t gfx_create_texture(bitmap_t* bitmap);
void gfx_texture(gfx_texture_t* texture);
void gfx_clear(vec4_t color);
void gfx_ortho_projection(window_t* window, f32 left, f32 right, f32 bottom, f32 top);
void gfx_ortho_projection_centered(window_t* window, f32 width, f32 height);
void gfx_color(v4 color);
void gfx_point(vec2_t pos);
void gfx_quad(vec2_t pos, vec2_t size);
void gfx_sprite(window_t* window, vec2_t pos, int px, int py, int pxs, int pys, float scale);
void gfx_sprite_tile(window_t* window, gfx_sprite_t* sprite, vec2_t pos, int tile);
void gfx_circle(vec2_t pos, f32 size, int segments);
void gfx_line_circle(vec2_t pos, f32 size, int segments);
void gfx_line(vec2_t start, vec2_t end);
void gfx_text(window_t* window, vec2_t pos, float scale, char* str, ...);


#endif