
// #include <windows.h>
// #include <gl/gl.h>
// #include <file.h>

// typedef GLuint glh_t;

#include "font/default_font.h"

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

gfx_texture_t* _gfx_active_texture = NULL;
vec2_t _gfx_coord_system = {1.0f, 1.0f};

#define gfx_color(c) glColor4f(c.r, c.g, c.b, c.a)

gfx_texture_t gfx_create_texture(bitmap_t* image) {
	gfx_texture_t result;
	result.width = image->width;
	result.height = image->height;
	glGenTextures(1, &result.handle);
	glBindTexture(GL_TEXTURE_2D, result.handle);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, image->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	return result;
}

gfx_texture_t gfx_create_null_texture(int width, int height) {
	// r_texture result;
	// result.width = image.header->bitmapWidth;
	// result.height = image.header->bitmapHeight;
	u32* data = malloc(sizeof(u32) * width * height);
	for(int y=0; y<height; ++y) for(int x=0; x<width; ++x) {
		if(((y/8)&1) ? (x / 8) & 1 : !((x / 8) & 1)) {
			data[y*width+x] = (255<<0) | (0) | (255<<16) | (255<<24);
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

void gfx_coord_system(f32 width, f32 height) {
	_gfx_coord_system = vec2(1.0f / width, 1.0f / height);
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

void gfx_sprite(core_window_t* window, vec2_t pos, int px, int py, int pxs, int pys, float scale) {
	vec2_t percent_of_screen = vec2(1.0f/((f32)window->width/pxs), 1.0f/((f32)window->height/pys));
	vec2_t s = {
		(percent_of_screen.x/_gfx_coord_system.x) * (f32)scale,
		(percent_of_screen.y/_gfx_coord_system.y) * (f32)scale,
	};
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

void gfx_sprite_tile(core_window_t* window, gfx_sprite_t sprite, vec2_t pos, int tile) {
	gfx_texture(sprite.texture);
	int tiles_per_row = sprite.texture->width / sprite.tile_size;
	int tiles_per_column = sprite.texture->height / sprite.tile_size;
	tile %= (tiles_per_row * tiles_per_column);
	gfx_sprite(window, pos,
			(tile%tiles_per_row)*sprite.tile_size,
			(tile/tiles_per_row)*sprite.tile_size,
			sprite.tile_size, sprite.tile_size, sprite.scale);
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

void gfx_text(core_window_t* window, vec2_t pos, float scale, char* str, ...) {
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
	vec2_t percent_of_screen = vec2(1.0f/((f32)window->width/8), 1.0f/((f32)window->height/8));
	vec2_t s = {
		(percent_of_screen.x/_gfx_coord_system.x) * (f32)scale,
		(percent_of_screen.y/_gfx_coord_system.y) * (f32)scale,
	};
	gfx_texture_t* t = _gfx_active_texture;
	
	// float charSize = 1.0f*0.05f;
	char* buffer = b;
	for (int i=0; *buffer; ++i,++buffer) {
		if(*buffer != '\n') {
			vec2_t uv = vec2((float)(*buffer%16) / 16.0f, (float)(*buffer/16) / 8.0f);
			vec2_t uvt = vec2(1.0f/16.0f, 1.0f/8.0f);
			vec2_t charPos = add2(pos, vec2(i * s.x, 0));
			glBegin(GL_QUADS);
			glTexCoord2f(uv.x,       uv.y+uvt.y); glVertex2f(charPos.x,            charPos.y+s.y);
			glTexCoord2f(uv.x+uvt.x, uv.y+uvt.y); glVertex2f(charPos.x+(s.x), charPos.y+s.y);
			glTexCoord2f(uv.x+uvt.x, uv.y);       glVertex2f(charPos.x+(s.x), charPos.y);
			glTexCoord2f(uv.x,       uv.y);       glVertex2f(charPos.x,            charPos.y);
			glEnd();
		}
	}
}

