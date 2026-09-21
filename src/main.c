#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <math.h>
#include <float.h>

#define PI 3.1415926535
#define ONE_DEG 0.0174533 // one degree in radians

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512
#define MAP_SIZE 64

struct map_t {
    int x;
    int y;
    int* array;
};

struct player_t {
    SDL_FRect* rect;
    float dx;
    float dy;
    float angle;
};

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    struct player_t* player;
    struct map_t* map;
} AppState;

bool init_player(struct player_t** player) {
    *player = SDL_calloc(1, sizeof(struct player_t));
    if (*player == NULL) {
        return false;
    }

    (*player)->rect = SDL_malloc(sizeof(SDL_FRect));
    if ((*player)->rect == NULL) {
        SDL_free(*player);
        *player = NULL;
        return false;
    }

    (*player)->rect->w = 8;
    (*player)->rect->h = 8;
    (*player)->rect->x = WINDOW_WIDTH / 4;
    (*player)->rect->y = WINDOW_HEIGHT / 2;
    (*player)->angle = 3 * PI / 2;

    return true;
}

bool init_map(struct map_t** map) {
    *map = SDL_malloc(sizeof(struct map_t));
    if (*map == NULL) {
        return false;
    }

    (*map)->x = 8;
    (*map)->y = 8;
    (*map)->array = SDL_calloc((*map)->x * (*map)->y, sizeof(int));

    if ((*map)->array == NULL) {
        SDL_free(*map);
        *map = NULL;
        return false;
    }

    int map_src[] = {
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1,
        1, 0, 0, 0, 0, 1, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1
    };

    SDL_memcpy((*map)->array, map_src, sizeof(map_src));

    return true;
}

SDL_AppResult handle_key(AppState* state, SDL_Keycode key) {
    if (state == NULL) return SDL_APP_FAILURE;

    struct player_t* player = state->player;
    if (!player || !player->rect) return SDL_APP_CONTINUE;

    switch (key) {
        case (SDLK_W):
            player->rect->x += player->dx;
            player->rect->y += player->dy;
            break;
        case (SDLK_S):
            player->rect->x -= player->dx;
            player->rect->y -= player->dy;
            break;
        case (SDLK_A):
            player->angle -= ONE_DEG * 8;
            if (player->angle < 0) {
                player->angle += 2 * PI;
            }
            player->dx = cos(player->angle) * 5;
            player->dy = sin(player->angle) * 5;
            break;
        case (SDLK_D):
            player->angle += ONE_DEG * 8;
            if (player->angle > 2 * PI) {
                player->angle -= 2 * PI;
            }
            player->dx = cos(player->angle) * 5;
            player->dy = sin(player->angle) * 5;
            break;
        case (SDLK_ESCAPE):
            return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

float dist_squared(float x1, float y1, float x2, float y2) {
    return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1);
}

float limit_angle(float angle) {
    if (angle <= 0) {
        angle += 2 * PI;
    } else if (angle >= 2 * PI) {
        angle -= 2 * PI;
    }
    return angle;
}

void draw_map(AppState* state) {
    if (state == NULL) return;

    struct map_t* map = state->map;
    if (map == NULL) return;

    int x_offset, y_offset;
    for (int y = 0; y < map->y; y++) {
        for (int x = 0; x < map->x; x++) {
            if (map->array[y*map->x + x] == 1) {
                SDL_SetRenderDrawColor(state->renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            } else {
                SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            }
            x_offset = x * MAP_SIZE;
            y_offset = y * MAP_SIZE;
            SDL_FRect rect;
            rect.x = x_offset + 2;
            rect.y = y_offset + 2;
            rect.w = MAP_SIZE - 2;
            rect.h = MAP_SIZE - 2;
            SDL_RenderFillRect(state->renderer, &rect);
        }
    }
}

void draw_player(AppState* state) {
    if (state == NULL) return;

    struct player_t* player = state->player;
    if (player == NULL) return;

    SDL_SetRenderDrawColor(state->renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(state->renderer, player->rect);
}

void draw_rays(AppState* state) {
    if (state == NULL) return;

    struct player_t* player = state->player;
    if (player == NULL || player->rect == NULL) return;

    struct map_t* map = state->map;
    if (map == NULL) return;

    int mx, my, mp, dof;
    float px, py, rx, ry, ra, xo, yo, dist;
    px = player->rect->x + (player->rect->w / 2);
    py = player->rect->y + (player->rect->h / 2);
    ra = player->angle - ONE_DEG * 30;

    for (int r = 0; r < 60; r++) {
        limit_angle(ra);
        /* ----- Check Horizontal Lines ----- */
        dof = 0;
        float atan = -1/tan(ra);
        float max_h = FLT_MAX;
        float hx = px;
        float hy = py;
        if (ra > PI) { // looking up
            ry = (((int)py>>6)<<6) - 0.0001;
            rx = (py - ry) * atan + px;
            yo = -64;
            xo = -yo * atan;
        } else if (ra < PI) { // looking down
            ry = (((int)py>>6)<<6) + 64;
            rx = (py - ry) * atan + px;
            yo = 64;
            xo = -yo * atan;
        } else { // looking straight left or right
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * map->x + mx;
            if (mp > 0 && mp < map->x*map->y && map->array[mp] == 1) {
                dof = 8; // hit the wall
                hx = rx;
                hy = ry;
                max_h = dist_squared(px, py, hx, hy);
            } else {
                rx += xo;
                ry += yo;
                dof += 1; // next line
            }
        }

        /* ----- Check Vertical Lines ----- */
        dof = 0;
        float ntan = -tan(ra);
        float max_v = FLT_MAX;
        float vx = px;
        float vy = py;
        if (ra > PI / 2 && ra < 3 * PI / 2) { // looking left
            rx = (((int)px>>6)<<6) - 0.0001;
            ry = (px - rx) * ntan + py;
            xo = -64;
            yo = -xo * ntan;
        } else if (ra < PI / 2 || ra > 3 * PI / 2) { // looking right
            rx = (((int)px>>6)<<6) + 64;
            ry = (px - rx) * ntan + py;
            xo = 64;
            yo = -xo * ntan;
        } else { // looking straight up or down
            rx = px;
            ry = py;
            dof = 8;
        }

        while (dof < 8) {
            mx = (int) (rx) >> 6;
            my = (int) (ry) >> 6;
            mp = my * map->x + mx;
            if (mp > 0 && mp < map->x*map->y && map->array[mp] == 1) {
                dof = 8; // hit the wall
                vx = rx;
                vy = ry;
                max_v = dist_squared(px, py, vx, vy);
            } else {
                rx += xo;
                ry += yo;
                dof += 1; // next line
            }
        }

        // only render whichever ray is shorter
        if (max_v < max_h) { // vertical wall hit
            rx = vx;
            ry = vy;
            dist = max_v;
        } else if (max_h < max_v) { // horizontal wall hit
            rx = hx;
            ry = hy;
            dist = max_h;
        }

        SDL_SetRenderDrawColor(state->renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderLine(state->renderer, px, py, rx, ry);
        ra += ONE_DEG;

        /* ----- Draw 3D Walls ----- */
        float ca = limit_angle(player->angle - ra);
        dist *= cos(ca); // fix fisheye effect
        float line_h = (MAP_SIZE*320) / sqrt(dist); // line height
        if (line_h > 320) {
            line_h = 320;
        }
        float lo = 160 - line_h / 2; // line offset

        if (max_v < max_h) {
            SDL_SetRenderDrawColor(state->renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        } else if (max_h < max_v) {
            SDL_SetRenderDrawColor(state->renderer, 0, 0, 150, SDL_ALPHA_OPAQUE);
        }
        SDL_FRect wall_piece;
        wall_piece.x = r * 8 + 530;
        wall_piece.y = lo;
        wall_piece.w = 8;
        wall_piece.h = line_h; 
        SDL_RenderFillRect(state->renderer, &wall_piece);
    }
}

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    AppState* state = (AppState*)SDL_calloc(1, sizeof(AppState));
    if (state == NULL) {
        return SDL_APP_FAILURE;
    }
    *appstate = state;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Raycaster", WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_WINDOW_RESIZABLE, &state->window,
                                     &state->renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(state->renderer, WINDOW_WIDTH, WINDOW_HEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);
    
    if (!init_player(&state->player) || !init_map(&state->map)) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppState* state = (AppState*)appstate;
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    } else if (event->type == SDL_EVENT_KEY_DOWN) {
        return handle_key(state, event->key.key);
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    AppState* state = (AppState*)appstate;
    // const Uint64 now = SDL_GetTicks();
    SDL_SetRenderDrawColor(state->renderer, 50, 50, 50, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(state->renderer);

    draw_map(state);
    draw_player(state);
    draw_rays(state);

    SDL_RenderPresent(state->renderer);

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    if (appstate != NULL) {
        AppState* state = (AppState*)appstate;

        if (state->player != NULL) {
            SDL_free(state->player->rect);
            SDL_free(state->player);
        }

        if (state->map != NULL) {
            SDL_free(state->map->array);
            SDL_free(state->map);
        }

        SDL_DestroyRenderer(state->renderer);
        SDL_DestroyWindow(state->window);
        SDL_free(state);
    }

    if (result == SDL_APP_FAILURE) {
        SDL_Log("Program terminated due to an error.");
    }
}