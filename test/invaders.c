
#include <core/core.h>
#include <core/video.h>
#include <core/im.h>
#include <core/timer.h>
// #include <core/file.h>
// #include <core/im.h>

#include <core/core.c>
#include <core/video.c>
#include <core/im.c>
#include <core/math.c>
#include <core/timer.c>
// #include <core/im.c>

float SCREEN_LEFT = -20;
float SCREEN_RIGHT = 20;
float SCREEN_BOTTOM = -20.0f*(600.0f/800.0f);
float SCREEN_TOP = 20.0f*(600.0f/800.0f);

#define ALIEN_ROW 12
#define ALIEN_COL 5

typedef struct {
    v2 pos;
} player_t;

typedef struct {
    b32 active;
    v2 pos;
} alien_t;

typedef struct {
    b32 active;
    v2 pos;
    v2 speed;
} bullet_t;

typedef struct {
    player_t player;
    alien_t aliens[ALIEN_ROW*ALIEN_COL];
    bullet_t player_bullet;
    bullet_t bullets[64];
    int bullet_count;
    allocator_t memory;
    gfx_sprite_t spritesheet;
} state_t;

void add_bullet(state_t* state, v2 pos, b32 player) {
    FOR (i, array_size(state->bullets)) {
        if (!state->bullets[i].active) {
            state->bullets[i] = (bullet_t){
                .active = TRUE,
                .pos = pos,
                .speed = vec2(0, player ? 1.0f : -1.0f),
            };
            return;
        }
    }
}

void player_shoot(state_t* state) {
    if (!state->player_bullet.active) {
        state->player_bullet = (bullet_t){
            .active = TRUE,
            .pos = state->player.pos,
            .speed = vec2(0, 1.0f),
        };
    }
}

int main() {
    print("Invaders");

    window_t window;
    start_window(&window, "Invaders", /*800.0f*1.5f, 600.0f*1.5f*/800, 600, WINDOW_CENTERED);
    start_opengl(&window);
    start_opengl_debug();

    state_t state = {0};
    state.memory = create_allocator(NULL, MB(10));
    state.player.pos = vec2(0, -12);

    timer_t timer = create_timer();

    // bitmap_t* bmp = load_bitmap_file(&state.memory, "spritesheet.bmp");
    // gfx_texture_t tex = gfx_create_texture(bmp);
    state.spritesheet = (gfx_sprite_t){
        // .texture = gfx_create_null_texture(64, 64),
        .texture = gfx_create_texture(load_bitmap_file(&state.memory, "spritesheet.bmp")),
        .tile_size = 16,
        .scale = 3,
    };

#define ALIEN_SPACING 2.5f
    FOR (r, ALIEN_ROW)
    FOR (c, ALIEN_COL) {
        state.aliens[c*ALIEN_ROW + r] = (alien_t){
            .active = TRUE,
            .pos = vec2(-ALIEN_SPACING*(ALIEN_ROW/2) + (ALIEN_SPACING/2.0f) + (ALIEN_SPACING*r), ALIEN_SPACING*c),
        };
    }

    while (!window.quit) {
        update_window(&window);
        update_timer(&timer);

        // gfx_coord_system(20, 20*(600/800));
        gfx_ortho_projection(&window, SCREEN_LEFT, SCREEN_RIGHT, SCREEN_BOTTOM, SCREEN_TOP);
        // glMatrixMode(GL_PROJECTION);
        // glLoadIdentity();
        // glOrtho(-20, 20, -20.0f*(600.0f/800.0f), 20.0f*(600.0f/800.0f), -10, 10);
        // glMatrixMode(GL_MODELVIEW);
        // glClear(GL_COLOR_BUFFER_BIT);
        gfx_clear(vec4(0,0.0f,0,0));

        player_t* player = &state.player;
        if (window.keyboard[KEY_LEFT].down) {
            player->pos.x -= 20.0f * timer.dt;
        }
        if (window.keyboard[KEY_RIGHT].down) {
            player->pos.x += 20.0f * timer.dt;
        }
        if (window.keyboard[KEY_SPACE].pressed) {
            // add_bullet(&state, player->pos, TRUE);
            player_shoot(&state);
        }

        // glPushMatrix();
        // glTranslatef(player->pos.x, player->pos.y, 0);
        // glColor4f(1, 0.5, 1, 1);
        // glBegin(GL_QUADS);
        // glVertex2f(-0.5f, -0.5f);
        // glVertex2f(0.5f, -0.5f);
        // glVertex2f(0.5f, 0.5f);
        // glVertex2f(-0.5f, 0.5f);
        // glEnd();
        // glPopMatrix();

        gfx_color(vec4(1, 0.5, 1, 1));
        gfx_quad(player->pos, vec2(1, 1));

        FOR (i, 1 + array_size(state.bullets)) {
            bullet_t* bullet = &state.player_bullet + i;
            if (bullet->active) {
                bullet->pos.y += bullet->speed.y * timer.dt * 60.0;

                glPushMatrix();
                glTranslatef(bullet->pos.x, bullet->pos.y, 0);
                glColor4f(1, 0.5, 1, 1);
                glBegin(GL_QUADS);
                glVertex2f(-0.05f, -0.5f);
                glVertex2f(0.05f, -0.5f);
                glVertex2f(0.05f, 0.5f);
                glVertex2f(-0.05f, 0.5f);
                glEnd();
                glPopMatrix();
            }
        }

        glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        gfx_texture(&state.spritesheet.texture);
        // glDisable(GL_TEXTURE_2D);
		// glBindTexture(GL_TEXTURE_2D, 0);
        // gfx_sprite(&window, vec2(0, 0), 0, 0, 64, 64, 4);

        FOR (r, ALIEN_ROW)
        FOR (c, ALIEN_COL) {
            alien_t* alien = state.aliens + c*ALIEN_ROW + r;

            if (alien->active) {
                if (len2(sub2(state.player_bullet.pos, alien->pos)) < 1.0f) {
                    alien->active = FALSE;
                    state.player_bullet.active = FALSE;
                }

                gfx_sprite_tile(
                    &window,
                    &state.spritesheet,
                    state.aliens[c*ALIEN_ROW + r].pos,
                    0
                );
            }
        }
        gfx_texture(NULL);

        opengl_swap_buffers(&window);
    }

    exit(0);
}