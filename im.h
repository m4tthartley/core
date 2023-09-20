
vec2_t _gfx_coord_system = {};

#define gfx_color(c) glColor4f(c.r, c.g, c.b, c.a)

void gfx_clear(vec4_t color) {
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

// void gfx_color(vec4_t color) {
// 	glColor4f(color.r, color.g, color.b, color.a);
// }

gfx_coord_system(f32 width, f32 height) {
	_gfx_coord_system = vec2(1.0f / width, 1.0f / height);
}

void gfx_quad(vec3_t pos, vec2_t size) {
	vec2_t s = {
		size.x/2.0f,
		size.y/2.0f,
	};
	glBegin(GL_QUADS);
	glVertex2f((pos.x-s.x) * _gfx_coord_system.x, pos.y-s.y * _gfx_coord_system.y);
	glVertex2f((pos.x+s.x) * _gfx_coord_system.x, pos.y-s.y * _gfx_coord_system.y);
	glVertex2f((pos.x+s.x) * _gfx_coord_system.x, pos.y+s.y * _gfx_coord_system.y);
	glVertex2f((pos.x-s.x) * _gfx_coord_system.x, pos.y+s.y * _gfx_coord_system.y);
	glEnd();
}
