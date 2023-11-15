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

#include "font/default_font.h"

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
