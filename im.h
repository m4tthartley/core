
// #include <windows.h>
// #include <gl/gl.h>
#include <file.h>

// typedef GLuint glh_t;

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
vec2_t _gfx_coord_system = {};

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
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture->handle);
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

void gfx_quad(vec3_t pos, vec2_t size) {
	glDisable(GL_TEXTURE_2D);

	vec2_t s = {
		size.x/2.0f,
		size.y/2.0f,
	};
	glBegin(GL_QUADS);
	glVertex2f((pos.x-s.x) * _gfx_coord_system.x, (pos.y-s.y) * _gfx_coord_system.y);
	glVertex2f((pos.x+s.x) * _gfx_coord_system.x, (pos.y-s.y) * _gfx_coord_system.y);
	glVertex2f((pos.x+s.x) * _gfx_coord_system.x, (pos.y+s.y) * _gfx_coord_system.y);
	glVertex2f((pos.x-s.x) * _gfx_coord_system.x, (pos.y+s.y) * _gfx_coord_system.y);
	glEnd();
}

void gfx_sprite(vec2_t pos, int px, int py, int pxs, int pys, int scale) {
	vec2_t pixel_size = vec2(1.0f/(_window_width/pxs), 1.0f/(_window_height/pys));
	vec2_t s = {
		(pixel_size.x/_gfx_coord_system.x)/2.0f * (f32)scale,
		(pixel_size.y/_gfx_coord_system.y)/2.0f * (f32)scale,
		// 1,1
	};
	gfx_texture_t* t = _gfx_active_texture;
	glBegin(GL_QUADS);
	glTexCoord2f((f32)px / (f32)t->width, ((f32)py) / (f32)t->height);
	// glTexCoord2f(0, 0);
	glVertex2f((pos.x-s.x) * _gfx_coord_system.x, (pos.y-s.y) * _gfx_coord_system.y);
	glTexCoord2f(((f32)px+pxs) / (f32)t->width, ((f32)py) / (f32)t->height);
	// glTexCoord2f(1, 0);
	glVertex2f((pos.x+s.x) * _gfx_coord_system.x, (pos.y-s.y) * _gfx_coord_system.y);
	glTexCoord2f(((f32)px+pxs) / (f32)t->width, ((f32)py+pys) / (f32)t->height);
	// glTexCoord2f(1, 1);
	glVertex2f((pos.x+s.x) * _gfx_coord_system.x, (pos.y+s.y) * _gfx_coord_system.y);
	glTexCoord2f((f32)px / (f32)t->width, ((f32)py+pys) / (f32)t->height);
	// glTexCoord2f(0, 1);
	glVertex2f((pos.x-s.x) * _gfx_coord_system.x, (pos.y+s.y) * _gfx_coord_system.y);
	glEnd();

}

void gfx_sprite_tile(gfx_sprite_t sprite, vec2_t pos, int tile) {
	gfx_texture(sprite.texture);
	int tiles_per_row = sprite.texture->width / sprite.tile_size;
	int tiles_per_column = sprite.texture->height / sprite.tile_size;
	tile %= (tiles_per_row * tiles_per_column);
	gfx_sprite(pos,
			(tile%tiles_per_row)*sprite.tile_size,
			(tile/tiles_per_row)*sprite.tile_size,
			sprite.tile_size, sprite.tile_size, sprite.scale);
}

void gfx_circle(vec3_t pos, f32 size, int segments) {
	glDisable(GL_TEXTURE_2D);

	vec2_t s = {
		size/2.0f,
		size/2.0f,
	};
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f((pos.x) * _gfx_coord_system.x, (pos.y) * _gfx_coord_system.y);
	for(int i=0; i<segments; ++i) {
		glVertex2f(
				(pos.x+sinf(PI2*((f32)i/segments))*s.x) * _gfx_coord_system.x,
				(pos.y+cosf(PI2*((f32)i/segments))*s.y) * _gfx_coord_system.y);
	}
	glVertex2f(
			(pos.x+sinf(PI2)*s.x) * _gfx_coord_system.x,
			(pos.y+cosf(PI2)*s.y) * _gfx_coord_system.y);
	glEnd();
}

void gfx_line(vec2_t start, vec2_t end) {
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	glVertex2f(start.x * _gfx_coord_system.x, start.y * _gfx_coord_system.y);
	glVertex2f(end.x * _gfx_coord_system.x, end.y * _gfx_coord_system.y);
	glEnd();
}

