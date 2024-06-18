//
//  invaders.c
//  Core
//
//  Created by Matt Hartley on 07/06/2024.
//  Copyright 2024 GiantJelly. All rights reserved.
//

/* 
    TODO
    - Decrease framebuffer size to 1to1
    - Software rendering
    - Update sprite api,
        I think the spritesheet structure is not needed
        and you can set the scale of the sprite rendering via a global state
        e.g. gfx_sprite_scale(3.0f);
    - Player lives and death
    - Fail state
    - Score
    - Intro screen
    - Flying saucer?
    - Embedded font system
    - Move core to single header again
    - Separate bitmap from general gfx code
 */

#define GL_SILENCE_DEPRECATION

#include <core/core.h>
#include <core/video.h>
// #include <core/file.h>
// #include <core/im.h>

#include <core/core.c>
#include <core/video.c>
#include <core/math.c>
// #include <core/im.c>

#define CORE_IMPL
#include <core/time.h>
#include <core/gfx.h>

#include "words.c"

float SCREEN_LEFT = -20;
float SCREEN_RIGHT = 20;
float SCREEN_BOTTOM = -20.0f*(600.0f/800.0f);
float SCREEN_TOP = 20.0f*(600.0f/800.0f);
float PIXEL_SCALE = 1.0f;

#define ALIEN_ROW_SIZE 11
#define ALIEN_COL_SIZE 5

typedef struct {
    b32 active;
	v2 pos;
    int lives;
    float respawn_timer;
} player_t;

typedef enum {
	ALIEN_TYPE_1 = 0,
	ALIEN_TYPE_2 = 1,
	ALIEN_TYPE_3 = 2,
} alien_type_t;
typedef struct {
	b32 active;
	alien_type_t type;
	v2 pos;
	v2 target_pos;
	int animation;
} alien_t;

typedef struct {
	b32 active;
	v2 pos;
	v2 speed;
	float animation;
} bullet_t;

typedef struct {
	b32 active;
	v2 pos;
	v2 speed;
	v4 color;
	f32 life;
	f32 lifetime;
} particle_t;

typedef struct {
	v2 pos;
} barrier_t;

typedef struct {
	player_t player;
	alien_t aliens[ALIEN_ROW_SIZE*ALIEN_COL_SIZE];
	bullet_t player_bullet;
	bullet_t bullets[64];
	particle_t particles[1024];
	int next_particle_slot;
	barrier_t barriers[4];

	float alien_move_timer;
	float alien_move_interval;
	float alien_move_direction;
	b32 alien_move_shift_down;
	int alien_move_row;

    float restart_timer;
} game_t;

typedef struct {
	allocator_t memory;

	bitmap_t* spritesheet_bmp;
	bitmap_t* font_bitmap;
    gfx_texture_t texture;
	gfx_sprite_t spritesheet;
    gfx_texture_t old_font_texture;
	gfx_texture_t font_texture;

	bitmap_t* barrier_bitmaps[4];
	gfx_texture_t barrier_textures[4];

    gfx_framebuffer_t framebuffer;

	game_t game;
} state_t;

void add_bullet(game_t* game, v2 pos) {
	FOR (i, array_size(game->bullets)) {
		if (!game->bullets[i].active) {
			game->bullets[i] = (bullet_t){
				.active = TRUE,
				.pos = pos,
				.speed = vec2(0, -1.0f),
			};
			return;
		}
	}
}

void add_background_particle(game_t* game, v2 pos, v2 speed) {
	game->particles[game->next_particle_slot] = (particle_t){
		.active = TRUE,
		.pos = pos,
		.speed = speed,
		.life = randfr(5.0f, 10.0f),
		.color = vec4(1.0f, 1.0f, 1.0f, randfr(0.0f, 0.2f)),
	};

	++game->next_particle_slot;
	game->next_particle_slot %= array_size(game->particles);
}

void add_damage_particle(game_t* game, v2 pos, v2 speed, v4 color) {
	game->particles[game->next_particle_slot] = (particle_t){
		.active = TRUE,
		.pos = pos,
		.speed = speed,
		.lifetime = 1.0f,
		.life = randfr(1.0f, 4.0f),
		.color = color,
	};

	++game->next_particle_slot;
	game->next_particle_slot %= array_size(game->particles);
}

void player_shoot(game_t* game) {
	if (!game->player_bullet.active) {
		game->player_bullet = (bullet_t){
			.active = TRUE,
			.pos = game->player.pos,
			.speed = vec2(0, 1.0f),
		};
	}
}

void explode_sprite(state_t* state, int tile, v2 pos) {
    FOR (y, 16) FOR (x, 16) {
        u32* pixels = state->spritesheet_bmp->data;
        // int tile = (c%3 * 2) + alien->animation;
        i32 p = pixels[((tile/4)*16 + y)*state->spritesheet_bmp->width + ((tile%4)*16 + x)];
        char* cc = (char*)&p;
        if (p) {
            // _gfx_ortho_res_scale
            add_damage_particle(
                &state->game,
                add2(pos, mul2(sub2f(vec2(x, y), 7.5f), mul2f(_gfx_ortho_res_scale, PIXEL_SCALE))),
                add2(mul2f(normalize2(sub2(vec2(x, y), vec2(8, 8))), 0.5f), vec2(randfr(-1.0f, 1.0f), randfr(-1.0f, 1.0f))),
                // vec2(0, 0),
                vec4((float)(p>>16&0xff) / 255.0f, (float)(p>>8&0xff) / 255.0f, (float)(p>>0&0xff) / 255.0f, 1.0f)
            );
        }
    }
}

state_t* start_system() {
	allocator_t _allocator = create_allocator(NULL, MB(10));
	state_t* state = alloc_memory_in(&_allocator, sizeof(state_t));
	zero_memory(state, sizeof(state_t));
	state->memory = _allocator;

	state->spritesheet_bmp = load_bitmap_file(&state->memory, "spritesheet.bmp");
	// gfx_texture_t tex = gfx_create_texture(bmp);
    state->texture = gfx_create_texture(state->spritesheet_bmp);
	state->spritesheet = (gfx_sprite_t){
		// .texture = gfx_create_null_texture(64, 64),
		.texture = state->texture,
		.tile_size = 16,
		.scale = PIXEL_SCALE,
	};

	state->font_bitmap = load_font_file(&state->memory, "font_v5.bmp");
	state->old_font_texture = gfx_create_texture(state->font_bitmap);
    state->font_texture = gfx_generate_font_texture(&state->memory, &FONT_DEFAULT);

	// Generate barrier bitmaps
	FOR (i, 4) {
		state->barrier_bitmaps[i] = alloc_memory_in(&state->memory, sizeof(bitmap_t)+(sizeof(u32)*16*16));
		state->barrier_bitmaps[i]->size = sizeof(u32)*16*16;
		state->barrier_bitmaps[i]->width = 16;
		state->barrier_bitmaps[i]->height = 16;

		FOR (y, 16) FOR (x, 16) {
			state->barrier_bitmaps[i]->data[y*16+x] = state->spritesheet_bmp->data[(32+y)*state->spritesheet_bmp->width + (32+x)];
		}

		state->barrier_textures[i] = gfx_create_texture(state->barrier_bitmaps[i]);
	}

	glPointSize(PIXEL_SCALE);

	return state;
}

void start_game(game_t* game) {
	zero_memory(game, sizeof(game_t));
	game->player.pos = vec2(0, -13);

	game->alien_move_interval = 0.2f;
	game->alien_move_timer = game->alien_move_interval;
	game->alien_move_direction = 1.0f;

    game->player.active = TRUE;
    game->player.lives = 3;

    game->restart_timer = 5.0f;
	
#define ALIEN_SPACING 3.0f
	FOR (r, ALIEN_ROW_SIZE)
	FOR (c, ALIEN_COL_SIZE) {
		game->aliens[c*ALIEN_ROW_SIZE + r] = (alien_t){
			.active = TRUE,
			.pos = vec2(-ALIEN_SPACING*((f32)ALIEN_ROW_SIZE/2) + (ALIEN_SPACING/2.0f) + (ALIEN_SPACING*r), ALIEN_SPACING*c),
			.target_pos = vec2(-ALIEN_SPACING*((f32)ALIEN_ROW_SIZE/2) + (ALIEN_SPACING/2.0f) + (ALIEN_SPACING*r), ALIEN_SPACING*c),
		};
	}

	FOR (i, array_size(game->barriers)) {
		game->barriers[i].pos = vec2(-12.0f + (8.0f*i), -10.0f);
	}
}

int main() {
	window_t window = {0};
	start_window(&window, "Galactic Conquerors", /*800.0f*1.5f, 600.0f*1.5f*/800, 600, WINDOW_CENTERED);
	start_opengl(&window);
	start_opengl_debug();

    // window.width = 200;
    // window.height = 150;
	
	state_t* state = start_system();
	start_game(&state->game);
	gametime_t time = time_init();

    state->framebuffer = gfx_create_framebuffer(200, 150, GFX_FORMAT_RGBA, GFX_SAMPLING_NEAREST);
    // gfx_bind_window_framebuffer(&window);
    gfx_bind_framebuffer(&state->framebuffer);

	// glEnable(GL_ALPHA_TEST);
	// glAlphaFunc(GL_GREATER, 0.0f);
    // glDisable(GL_BLEND);
    // glDisable(GL_ALPHA_TEST);

	while (!window.quit) {
		update_window(&window);
		time_update(&time);

		game_t* game = &state->game;

        gfx_bind_framebuffer(&state->framebuffer);
		gfx_ortho_projection(200, 150, SCREEN_LEFT, SCREEN_RIGHT, SCREEN_BOTTOM, SCREEN_TOP);
		gfx_clear(vec4(0.0f, 0.0f, 0.0f, 0.0f));

        glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        gfx_sprite_scale(1.0f);
        gfx_sprite_tile_size(16);
        gfx_texture(&state->texture);

        // PLAYER
        player_t* player = &game->player;
        if (player->active) {
            if (window.keyboard[KEY_LEFT].down) {
                player->pos.x -= 10.0f * time.dt;
            }
            if (window.keyboard[KEY_RIGHT].down) {
                player->pos.x += 10.0f * time.dt;
            }
            if (window.keyboard[KEY_SPACE].pressed) {
                player_shoot(game);
            }

            gfx_color(vec4(1, 1, 1, 1));
            // gfx_quad(player->pos, vec2(1, 1));
            // gfx_sprite_tile(
            //     &window,
            //     &state->spritesheet,
            //     player->pos,
            //     8
            // );
            gfx_draw_sprite_tile(player->pos, 8);

            FOR (i, array_size(game->bullets)) {
                bullet_t* bullet = game->bullets + i;
                if (bullet->active) {
                    if (len2(sub2(bullet->pos, player->pos)) < 1.0f) {
                        bullet->active = FALSE;
                        player->active = FALSE;
                        player->respawn_timer = 2.0f;
                        --player->lives;
                        explode_sprite(state, 8, player->pos);
                        player->pos.x = 0.0f;
                    }
                }
            }

        } else {
            player->respawn_timer -= time.dt;
            if (player->lives && player->respawn_timer < 0.0f) {
                player->active = TRUE;
            }
        }

		// PARTICLES
		gfx_texture(NULL);
		add_background_particle(game,
			vec2(randf_range(SCREEN_LEFT, SCREEN_RIGHT), randf_range(SCREEN_BOTTOM, SCREEN_TOP)),
			vec2(randf_range(-1.0f, 1.0f), randf_range(-1.0f, 1.0f)));

		int active_particles = 0;
		FOR (i, array_size(game->particles)) {
			particle_t* particle = game->particles + i;
			if (particle->active) {
				++active_particles;

				if (particle->life < 0.0f) {
					particle->active = FALSE;
				}

				particle->life -= time.dt;
				particle->lifetime += time.dt;

				v4 color = particle->color;
				color.a = particle->lifetime < 1.0f ? min(particle->lifetime, color.a) : min(particle->life, color.a);
				gfx_color(color);
				gfx_point(particle->pos);
				
				particle->pos = add2(particle->pos, mul2f(particle->speed, time.dt));
			}
		}

		// BULLETS
		// gfx_texture(NULL);
        gfx_color(vec4(1, 1, 1, 1));
		FOR (i, 1 + array_size(game->bullets)) {
			bullet_t* bullet = &game->player_bullet + i;
			if (bullet->active) {
				bullet->pos.y += bullet->speed.y * time.dt * 20.0;
				bullet->animation += 20.0f * time.dt;
				if (bullet->animation >= 4.0f) bullet->animation -= 4.0f;

				if (bullet->pos.x < SCREEN_LEFT ||
					bullet->pos.x > SCREEN_RIGHT ||
					bullet->pos.y < SCREEN_BOTTOM ||
					bullet->pos.y > SCREEN_TOP) {
					bullet->active = FALSE;
				}

				FOR (bi, array_size(game->barriers)) {
					barrier_t* barrier = game->barriers + bi;

					// v2 pixel_overlapf = add2f(mul2(sub2(barrier->pos, bullet->pos), div2f(_gfx_ortho_res_scale, PIXEL_SCALE)), 8.0f);
					v2 pixel_scale = mul2f(_gfx_ortho_res_scale, PIXEL_SCALE);
					v2 pixel_overlapf = add2f(div2(sub2(bullet->pos, barrier->pos), pixel_scale), 8.0f);
					i2 pixel_overlap = {pixel_overlapf.x, pixel_overlapf.y};
					if (pixel_overlap.x>=0 && pixel_overlap.x<16 &&
						pixel_overlap.y>=0 && pixel_overlap.y<16 /*len2(sub2(bullet->pos, barrier->pos)) < 2.0f*/) {

						bitmap_t* bitmap = state->barrier_bitmaps[bi];
						u32* pixel = bitmap->data + (pixel_overlap.y)*bitmap->width + (pixel_overlap.x);
						if (*pixel) {
							bullet->active = FALSE;
							
							// explosion
							FOR (y, 16) FOR (x, 16) {
								u32* pixels = bitmap->data;
								// int tile = 10;
								u32* p = pixels + (y)*bitmap->width + (x);
								v2 pixel_pos = add2(barrier->pos, mul2(sub2f(vec2(x, y), 7.5f), mul2f(_gfx_ortho_res_scale, PIXEL_SCALE)));
								
								// i2 pixel_overlap = {

								// };

								float dist = len2(sub2(bullet->pos, pixel_pos));
								if (dist < 0.5f && randf()<(1.0f - powf(dist*2.0f, 2.0f))) {
									*p = 0;
								}
							}

							gfx_update_texture(state->barrier_textures + bi, bitmap);
						}
						
						// barrier->pos.x += 1.0f;
						

						// state->spritesheet_bmp->data[(32+pixel_overlap.y)*state->spritesheet_bmp->width + (32+pixel_overlap.x)] = 0;
					}
				}

				// glPushMatrix();
				// glTranslatef(bullet->pos.x, bullet->pos.y, 0);
				// glColor4f(1, 1, 1, 1);
				// glBegin(GL_QUADS);
				// glVertex2f(-0.05f, -0.5f);
				// glVertex2f(0.05f, -0.5f);
				// glVertex2f(0.05f, 0.5f);
				// glVertex2f(-0.05f, 0.5f);
				// glEnd();
				// glPopMatrix();
				// gfx_sprite_tile(&window, &state->spritesheet, bullet->pos, 12 + (int)bullet->animation);
				gfx_texture(NULL);
				gfx_point(bullet->pos);
			}
		}

		// ALIENS
		game->alien_move_timer -= time.dt;
		if (game->alien_move_timer < 0.0f) {
			game->alien_move_timer = game->alien_move_interval;

			int row = game->alien_move_row;
			FOR (i, ALIEN_ROW_SIZE) {
				alien_t* alien = game->aliens + row*ALIEN_ROW_SIZE + i;

				if (game->aliens[row*ALIEN_ROW_SIZE + i].active) {
					++alien->animation;
					alien->animation %= 2;

					if (game->alien_move_shift_down) {
						alien->target_pos.y -= 1.0f;
					} else {
						alien->target_pos.x = alien->pos.x + game->alien_move_direction * 0.5f;
					}
				}
			}

			++game->alien_move_row;
			game->alien_move_row %= ALIEN_COL_SIZE;

			if (game->alien_move_row == 0) {
				game->alien_move_shift_down = FALSE;

				FOR (i, ALIEN_ROW_SIZE*ALIEN_COL_SIZE) {
					alien_t* alien = game->aliens + i;
					// alien->pos.y -= 0.5f;

					if (alien->active) {
						if (alien->pos.x > 17.0f) {
							if (game->alien_move_direction > 0.0f) {
								game->alien_move_direction = -1.0f;
								game->alien_move_shift_down = TRUE;
							}
						}
						if (alien->pos.x < -17.0f) {
							if (game->alien_move_direction < 0.0f) {
								game->alien_move_direction = 1.0f;
								game->alien_move_shift_down = TRUE;
							}
						}
					}
				}
			}
		}

		gfx_texture(&state->spritesheet.texture);
		// glDisable(GL_TEXTURE_2D);
		// glBindTexture(GL_TEXTURE_2D, 0);
		// gfx_sprite(&window, vec2(0, 0), 0, 0, 64, 64, 4);
        float lowest_alien = 0.0f;
		FOR (r, ALIEN_ROW_SIZE)
		FOR (c, ALIEN_COL_SIZE) {
			alien_t* alien = game->aliens + c*ALIEN_ROW_SIZE + r;
            if (alien->pos.y < lowest_alien) {
                lowest_alien = alien->pos.y;
            }

			if (alien->active) {
				if (len2(sub2(alien->target_pos, alien->pos)) > 0.1f) {
					// alien->pos = add2(alien->pos, mul2f(normalize2(sub2(alien->target_pos, alien->pos)), 5.0f*time.dt));
					alien->pos = alien->target_pos;
				}

				if (game->player_bullet.active && len2(sub2(game->player_bullet.pos, alien->pos)) < 1.0f) {
					alien->active = FALSE;
					game->player_bullet.active = FALSE;

					// Explosion
					// FOR (y, 16) FOR (x, 16) {
					// 	u32* pixels = state->spritesheet_bmp->data;
					// 	int tile = (c%3 * 2) + alien->animation;
					// 	i32 p = pixels[((tile/4)*16 + y)*state->spritesheet_bmp->width + ((tile%4)*16 + x)];
					// 	char* cc = (char*)&p;
					// 	if (p) {
					// 		// _gfx_ortho_res_scale
					// 		add_damage_particle(
					// 			game,
					// 			add2(alien->pos, mul2(sub2f(vec2(x, y), 7.5f), mul2f(_gfx_ortho_res_scale, PIXEL_SCALE))),
					// 			// vec2(randf_range(-0.5f, 0.5f), randf_range(-0.5f, 0.5f))
					// 			add2(mul2f(normalize2(sub2(vec2(x, y), vec2(8, 8))), 0.5f), vec2(randfr(-0.2f, 0.2f), randfr(-0.2f, 0.2f))),
					// 			// vec4(97.0f / 255.0f, 16.0f / 255.0f, 162.0f / 255.0f, 1.0f)
					// 			vec4((float)(p>>16&0xff) / 255.0f, (float)(p>>8&0xff) / 255.0f, (float)(p>>0&0xff) / 255.0f, 1.0f)
					// 		);
					// 	}
					// }
                    int tile = (c%3 * 2) + alien->animation;
                    explode_sprite(state, tile, alien->pos);
				}

				if (randf() < 0.0002f) {
					add_bullet(game, alien->pos);
				}

				gfx_draw_sprite_tile(
					game->aliens[c*ALIEN_ROW_SIZE + r].pos,
					(c%3 * 2) + alien->animation
				);
			}
		}
        
        // Lives
        // state->spritesheet.scale = 2.0f;
        gfx_sprite_scale(0.7f);
        FOR (i, game->player.lives) {
            gfx_draw_sprite_tile(
                vec2(SCREEN_LEFT + 1.5f + (2.0f*i), SCREEN_BOTTOM + 1.0f),
                8
            );
        }
        // state->spritesheet.scale = PIXEL_SCALE;
        gfx_sprite_scale(1);

		// BARRIERS
		FOR (i, array_size(game->barriers)) {
			barrier_t* barrier = game->barriers + i;
			// gfx_sprite_tile(&window, &state->, barrier->pos, 10);
			gfx_texture(state->barrier_textures + i);
			gfx_draw_sprite_rect(barrier->pos, vec2(0, 0), vec2(16, 16));
		}

		// TEXT
		gfx_color(vec4(1, 1, 1, 1));
		gfx_texture(&state->font_texture);
		
        static float scale = 1.0f;
        // scale += time.dt*0.5f;
        gfx_sprite_scale(scale);
		// gfx_text(&window, vec2(0, 0), "Invaders");
		// gfx_text(&window, vec2(SCREEN_LEFT + 1, SCREEN_TOP - 2), 2.0f, "Partilces %i", active_particles);
        // gfx_text(&window, vec2(SCREEN_LEFT + 1, SCREEN_TOP - 2), "Lowest alien %f", lowest_alien);
        static float scroll = 0.0f;
        static float wheel_momentum = 0.0f;
        if ((window.mouse.wheel_dt > 0 && window.mouse.wheel_dt > wheel_momentum) ||
            (window.mouse.wheel_dt < 0 && window.mouse.wheel_dt < wheel_momentum)) {
            wheel_momentum = (float)window.mouse.wheel_dt;
        } else {
            wheel_momentum *= 0.9f;
        }
        scroll -= wheel_momentum*0.5f;
        _gfx_font_wrap_width = 30.0f;
        int stuff = array_size(FONT_DEFAULT.combos);
        char* rwby =
            "Red like roses\n"
            "Fills my head with dreams and finds me\n"
            "Always closer\n"
            "To the emptiness and sadness\n"
            "That has come to take the place of you\n"
            "Amazing 0123456789 \n"
            "silly sausages  \n";
        gfx_draw_text(&FONT_DEFAULT, vec2(-18, 12 + scroll), ALL_THE_WORDS);
        // gfx_draw_text(&FONT_DEFAULT, vec2(0, 0), "a");
        gfx_sprite_scale(1.0f);

        if (!player->lives) {
            game->restart_timer -= time.dt;
            if (game->restart_timer < 0.0f) {
                start_game(game);
            }
            gfx_text(&window, vec2(SCREEN_LEFT + 1, SCREEN_TOP - 2), "Game Over");
        }

        // PRESENT
        gfx_bind_window_framebuffer(&window);

        glDisable(GL_BLEND);
	    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, state->framebuffer.textures[0]);
        // glBindTexture(GL_TEXTURE_2D, state->font_texture.handle);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);

        glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex2f( 1.0f, -1.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex2f( 1.0f,  1.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f,  1.0f);
        glEnd();

        // END
		GLenum gl_error = glGetError();
		if (gl_error) {
			print("OpenGL error: %i", gl_error);
		}

		opengl_swap_buffers(&window);
	}

	exit(0);
}