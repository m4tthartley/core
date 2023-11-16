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

#include "core.h"
#include "math.h"

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
	gfx_texture_t* texture;
	int tile_size;
	int scale;
} gfx_sprite_t;

#define gfx_color(c) glColor4f(c.r, c.g, c.b, c.a)

gfx_texture_t gfx_create_null_texture(int width, int height);
gfx_texture_t gfx_create_texture(bitmap_t* bitmap);
void gfx_texture(gfx_texture_t* texture);
void gfx_clear(vec4_t color);
void gfx_coord_system(f32 width, f32 height);
void gfx_point(vec2_t pos);
void gfx_quad(vec2_t pos, vec2_t size);
void gfx_sprite(core_window_t* window, vec2_t pos, int px, int py, int pxs, int pys, float scale);
void gfx_sprite_tile(core_window_t* window, gfx_sprite_t sprite, vec2_t pos, int tile);
void gfx_circle(vec2_t pos, f32 size, int segments);
void gfx_line_circle(vec2_t pos, f32 size, int segments);
void gfx_line(vec2_t start, vec2_t end);
void gfx_text(core_window_t* window, vec2_t pos, float scale, char* str, ...);
