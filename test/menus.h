
#include <core/gfx.h>

void do_menus(state_t* state, window_t* window, gametime_t* time) {
	game_t* game = &state->game;
	gfx_bind_framebuffer(&state->menu_framebuffer);
	gfx_clear(vec4(0.0f, 0.0f, 0.0f, 0.0f));
	gfx_color(vec4(1, 1, 1, 1));
	gfx_texture(&state->font_texture);

	// static float zoom = -1.0f;
	if (game->mode == GAMEMODE_MENU) {
		game->menu_zoom += max((3.0f-game->menu_zoom) * 1.0f, 0.5f) * time->dt;
		game->menu_zoom = min(game->menu_zoom, 2.0f);
	}
	if (game->mode == GAMEMODE_GAMEOVER) {
		game->menu_zoom += time->dt * 2.0f;
		game->menu_zoom = min(game->menu_zoom, 2.0f);
	}
	if (game->menu_zoom > 0.0f) {
		gfx_sprite_scale(game->menu_zoom);
		if (game->mode == GAMEMODE_MENU) {
			// v2 size = gfx_layout_text(&FONT_DEFAULT, "Galactic\nConquerors");
			// gfx_texture(NULL);
			// gfx_color(vec4(1, 0, 0, 1));
			// gfx_quad(vec2(0, 5), size);
			// gfx_texture(&state->font_texture);
			// gfx_color(vec4(1, 1, 1, 1));

			gfx_draw_text_centered(&FONT_DEFAULT, vec2(-7, 5), "Galactic\nConquerors");
		}
		if (game->mode == GAMEMODE_GAMEOVER) {
			gfx_draw_text_centered(&FONT_DEFAULT, vec2(0, 10), "Game Over");
		}
		gfx_sprite_scale(1.0f);
	}

	if (game->menu_zoom > (game->mode == GAMEMODE_MENU ? 1.99f : 1.99f)) {
		static int selected_item = 0;
		static float movement = 0.0f;

		static int selected_name_char = 0;
		static b32 name_mode = FALSE;
		static char name[4] = { 'A', 'A', 'A', 'A' };
		static b32 submitted = FALSE;

		movement += time->dt * 8.0f;

		gfx_sprite_scale(1.0f);
		char* menu_items[] = {
			"",
			"Play",
			"Exit",
		};
		if (game->mode == GAMEMODE_GAMEOVER) {
			menu_items[1] = "Submit";
			menu_items[2] = "Menu";
		}
		
		if (name_mode) {
			if (window->keyboard[KEY_DOWN].pressed) {
				++name[selected_name_char];
			}
			if (window->keyboard[KEY_UP].pressed) {
				--name[selected_name_char];
			}
			// name[selected_name_char] = max(min(name[selected_name_char], 'Z'), 'A');
			if (name[selected_name_char] < 'A') name[selected_name_char] += 26;
			if (name[selected_name_char] > 'Z') name[selected_name_char] -= 26;
		} else {
			if (window->keyboard[KEY_DOWN].pressed) {
				++selected_item;
			}
			if (window->keyboard[KEY_UP].pressed) {
				--selected_item;
			}
		}
		if (window->keyboard[KEY_RIGHT].pressed) {
			++selected_name_char;
		}
		if (window->keyboard[KEY_LEFT].pressed) {
			--selected_name_char;
		}
		if (window->keyboard[KEY_RETURN].released) {
			switch (selected_item) {
				case 0:
					if (game->mode == GAMEMODE_MENU) {
						switch_to_play(state, game);
					} else {
						if (selected_item==0) {
							name_mode = !name_mode;
						}
					}
					break;
				case 1:
					if (game->mode == GAMEMODE_MENU) {
						exit(0);
					} else {
						// switch_to_menu(game);
						submit_score((highscore){.name={name[0],name[1],name[2],name[3]}, .value=game->score});
						submitted = TRUE;
					}
					break;
			}
		}
		selected_item = min(max(selected_item, 0), array_size(menu_items)-1);
		selected_name_char = min(max(selected_name_char, 0), 3);

		if (game->mode == GAMEMODE_GAMEOVER) {
			if (!name_mode && selected_item==0) {
				gfx_color(lerp4(vec4(1, 1, 1, 1), vec4(1, 1, 0.5f, 1), sinf(movement)));
			}
			FOR (i, 4) {
				gfx_color(vec4(1, 1, 1, 1));
				if ((!name_mode && selected_item==0) || (name_mode && selected_item==0 && selected_name_char == i)) {
					gfx_color(lerp4(vec4(1, 1, 1, 1), vec4(1, 1, 0.5f, 1), sinf(movement)));
					if (name_mode) {
						gfx_draw_text_centered(&FONT_DEFAULT, vec2(-3 + 2.0f*i, -1.8f + sinf(movement)*0.3f), "^");
					}
				}
				gfx_draw_text_centered(&FONT_DEFAULT, vec2(-3 + 2.0f*i, 0), str_format("%.1s", name+i));
			}
		}

		gfx_color(vec4(1, 1, 1, 1));

		int menu_x = 0;
		int menu_y = -0;
		if (game->mode == GAMEMODE_MENU) {
			menu_x = -5;
			menu_y = -8;
		}
		FOR (i, array_size(menu_items)) {
			if (!name_mode && selected_item == i /*&& !(game->mode==GAMEMODE_GAMEOVER && i==0)*/) {
				gfx_color(lerp4(vec4(1, 1, 1, 1), vec4(1, 1, 0.5f, 1), sinf(movement)));
				gfx_draw_text_centered(&FONT_DEFAULT, vec2(menu_x-6.0f + (sinf(movement)*0.5f), menu_y - i*3.0f), ">");
				// gfx_color(lerp4(vec4(1, 1, 1, 1), vec4(1, 1, 0.5f, 1), sinf(movement)));
				gfx_draw_text_centered(&FONT_DEFAULT, vec2(menu_x+6.0f - (sinf(movement)*0.5f), menu_y - i*3.0f), "<");

				// gfx_color(lerp4(vec4(1, 1, 1, 1), vec4(1, 1, 0.5f, 1), sinf(movement)));
			} else {
				gfx_color(vec4(1, 1, 1, 1));
			}

			// v2 size = gfx_layout_text(&FONT_DEFAULT, menu_items[i]);
			// gfx_texture(NULL);
			// gfx_color(vec4(1, 0, 0, 1));
			// gfx_quad(vec2(0, -8 - i*3.0f), size);
			// gfx_texture(&state->font_texture);
			// gfx_color(vec4(1, 1, 1, 1));

			gfx_draw_text_centered(&FONT_DEFAULT, vec2(menu_x, menu_y - i*3.0f), menu_items[i]);
		}
	}

	gfx_color(vec4(1, 1, 1, 1));

	// gfx_font_wrap_width(100.0f);
	if (game->mode == GAMEMODE_MENU) {
		v2 size = gfx_layout_text(&FONT_DEFAULT, "NAME");
		gfx_draw_text(&FONT_DEFAULT, vec2(6, +10), "NAME");
		gfx_draw_text_right_aligned(&FONT_DEFAULT, vec2(18, +10), "SCORE");

		FOR (i, game->highscores.count) {
			highscore score = game->highscores.scores[i];
			gfx_draw_text(&FONT_DEFAULT, vec2(6, 7 - i*2.0f), str_format("%.4s", score.name));
			gfx_draw_text_right_aligned(&FONT_DEFAULT, vec2(18, 7 - i*2.0f), str_format("%u", score.value));
		}
	}
}