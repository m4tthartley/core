
// #include <stdarg.h>
// #include <stdio.h>

#include <platform.h>
#include <im.h>
#include <timer.h>
#include <file.h>
#include <audio.h>

typedef struct {
	core_window_t window;
	core_timer_t timer;
	core_audio_t audio;
	vec2_t pos;
	int2 speed;
	dynarr_t blocks;
	gfx_texture_t font;
	m_arena assets;

	audio_buffer_t* hit_sound;
} state_t;

typedef struct {
	float color;
} block_t;

void* start() {
	state_t* state = malloc(sizeof(state_t));
	m_zero(state, sizeof(state_t));

	core_window(&state->window, "Reloader Test", 800, 600, 0);
	core_opengl(&state->window);
	core_timer(&state->timer);

	state->pos = (vec2_t){0, 5};
	state->speed = (int2){1, 1};

	state->blocks = dynarr(sizeof(block_t));

	m_stack(&state->assets, 0, 0);
	m_reserve(&state->assets, GB(1), PAGE_SIZE);
	bitmap_t* font_data = f_load_font_file(&state->assets, "../core/font/font.bmp");
	state->font = gfx_create_texture(font_data);

	state->hit_sound = f_load_wave(&state->assets, "hit.wav");
	core_init_audio(&state->audio, CORE_DEFAULT_AUDIO_MIXER_PROC, 0);
	core_play_sound(&state->audio, state->hit_sound, 0.5f);

	return state;
}

void frame(void* param) {
	state_t* state = param;
	if (state->audio.reload) {
		state->audio.reload = FALSE;
		core_init_audio(&state->audio, CORE_DEFAULT_AUDIO_MIXER_PROC, 0);
	}
	core_window_update(&state->window);
	core_timer_update(&state->timer);
	gfx_coord_system(800.0f/64.0f, 600.0f/64.0f);
	// gfx_clear(vec4(sinf(core_time_seconds(&state->window))*0.5f+0.5f,0.8f,0.5f,0));
	gfx_clear(vec4(0, 1, 0, 0));
	gfx_color(vec4(1, 1, 1, 1));
	if(state->pos.x < -8.0f) {
		state->speed.x = 1;
		core_play_sound(&state->audio, state->hit_sound, 0.5f);
	}
	if(state->pos.x > 8.0f) {
		state->speed.x = -1;
		core_play_sound(&state->audio, state->hit_sound, 0.5f);
	}
	if(state->pos.y < -8.0f) {
		state->speed.y = 1;
		core_play_sound(&state->audio, state->hit_sound, 0.5f);
	}
	if(state->pos.y > 8.0f) {
		state->speed.y = -1;
		core_play_sound(&state->audio, state->hit_sound, 0.5f);
	}
	state->pos.x += (f32)state->speed.x * state->timer.dt * 5.0f;
	state->pos.y += (f32)state->speed.y * state->timer.dt * 5.0f;
	gfx_quad(state->pos, vec2(0.5f, 0.5f));

	float time = sinf(core_time_seconds(&state->window) * 1.0f) * 0.4f + 0.6f;
	// printf("%f \n", time);
	block_t block = {time};
	dynarr_push(&state->blocks, &block);

	if (state->blocks.count > 420) {
		FOR (i, 420) {
			dynarr_pop(&state->blocks, 0);
		}
	}

	block_t* b1 = dynarr_get(&state->blocks, state->blocks.count-1);
	// printf("%f \n", b1->color);

	FOR (i, state->blocks.count) {
		block_t* b = dynarr_get(&state->blocks, i);
		// printf("%f \n", *color);
		gfx_color(vec4(0, 0, b->color, 1));
		gfx_quad(vec2(-10.0f + (i % 21)*1.0f, -10.0f + (i / 21)), vec2(0.5f, 0.5f));
	}

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gfx_color(vec4(1, 1, 1, 1));
	gfx_texture(&state->font);
	gfx_text(&state->window, vec2(-11, 8), 2, "hello world");
	gfx_texture(NULL);

	core_opengl_swap(&state->window);
}

int main() {
	void* state = start(NULL);
	for(;;) {
		frame(state);
	}
}

