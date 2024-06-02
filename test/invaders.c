
#include <core/core.h>
#include <core/video.h>
#include <core/im.h>
#include <core/timer.h>
#include <core/file.h>
#include <core/im.h>

#include <core/core.c>
#include <core/video.c>
#include <core/im.c>
#include <core/math.c>
#include <core/timer.c>
#include <core/im.c>
        
typedef struct {
    v2 pos;
} player_t;

typedef struct {
    b32 active;
    v2 pos;
    v2 speed;
} bullet_t;

typedef struct {
    player_t player;
    bullet_t bullets[64];
    int bullet_count;
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

int main() {
    print("Invaders");

    window_t window;
    start_window(&window, "Invaders", 800, 600, WINDOW_CENTERED);
    start_opengl(&window);

    state_t state = {0};
    state.player.pos = vec2(0, -12);

    timer_t timer = create_timer();

    while (!window.quit) {
        update_window(&window);
        update_timer(&timer);

        gfx_coord_system(20, 20*(600/800));
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-20, 20, -20.0f*(600.0f/800.0f), 20.0f*(600.0f/800.0f), -10, 10);
        glMatrixMode(GL_MODELVIEW);
        glClear(GL_COLOR_BUFFER_BIT);

        player_t* player = &state.player;
        if (window.keyboard[KEY_LEFT].down) {
            player->pos.x -= 20.0f * timer.dt;
        }
        if (window.keyboard[KEY_RIGHT].down) {
            player->pos.x += 20.0f * timer.dt;
        }
        if (window.keyboard[KEY_SPACE].pressed) {
            add_bullet(&state, player->pos, TRUE);
        }

        glPushMatrix();
        glTranslatef(player->pos.x, player->pos.y, 0);
        glColor4f(1, 0.5, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.5f, -0.5f);
        glVertex2f(0.5f, 0.5f);
        glVertex2f(-0.5f, 0.5f);
        glEnd();
        glPopMatrix();

        FOR (i, array_size(state.bullets)) {
            if (state.bullets[i].active) {
                bullet_t* bullet = state.bullets + i;
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

        opengl_swap_buffers(&window);
    }

    exit(0);
}