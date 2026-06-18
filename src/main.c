#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "raylib.h"

#include "world.h"

#define MSG_TITLE "MONKEYS"
#define MSG_TITLE_DELIMITER " | "

#define S_TITLE_COUNTDOWN_INITIAL 0.1

#define FONT_SIZE_DEBUG     20
#define FONT_SIZE_EMPTYLINE FONT_SIZE_DEBUG/2

#define WIDTH         1600
#define HEIGHT        900
#define MINIMAP_SCALE 0.125f

#define CAM_ZOOM_DEFAULT  1.0f
#define CAM_MOVEMENT_STEP 10.0f

#define DRAW_TEXT_DEBUG(str, x, y) \
    DrawText( \
        str, \
        x+1,  \
        y+1, \
        FONT_SIZE_DEBUG, \
        RAYWHITE); \
    DrawText( \
        str, \
        x,  \
        y, \
        FONT_SIZE_DEBUG, \
        BLACK);

int main()
{
    // init
    InitWindow(WIDTH, HEIGHT, MSG_TITLE);
    SetTargetFPS(60);
    srand(time(NULL));
    // data
    World* world = world_new_initialized(WIDTH, HEIGHT);
    terrain_perlin(world->terrain, WIDTH, HEIGHT, 0.987f, 0.9f, 0xC7b9);
    Camera2D cam = {0};
    cam.zoom     = CAM_ZOOM_DEFAULT;
    bool is_spawning_monkeys = false;
    // counters
    uint64_t count_death     = 0;
    uint64_t count_newborn   = 0;
    DNA dna_avg              = {0};
    // start loop
    double s_prev            = GetTime();
    double s_title_countdown = S_TITLE_COUNTDOWN_INITIAL;
    // draw terrain
    RenderTexture2D terrain_rt = LoadRenderTexture(WIDTH, HEIGHT);
    BeginTextureMode(terrain_rt);
    ClearBackground(BLANK);
    for (int i=0; i<HEIGHT; ++i)
    {
        for (int j=0; j<WIDTH; ++j)
        {
            // draw terrain
            Color col = BLACK;
            switch (world->terrain[i*WIDTH + j])
            {
                #define _CASE(c,h) case c: col = GetColor(h); break
                _CASE(TERRAIN_EARTH,         0x7CBF58FF);
                _CASE(TERRAIN_STONE,         0x7A6D6DFF);
                _CASE(TERRAIN_WATER,         0xA9E2F5FF);
                _CASE(TERRAIN_WATER_SHALLOW, 0xB9EBFAFF);
                _CASE(TERRAIN_WATER_DEEP,    0x98D6EBFF);
                default: break;
                #undef _CASE
            }
            DrawPixel(j, i, col);

        }
    }
    EndTextureMode();
    while (!WindowShouldClose())
    {
        // update time
        double s_curr    = GetTime();
        double s_elapsed = s_curr - s_prev;
        s_prev           = s_curr;
        // update title
        s_title_countdown -= s_elapsed;
        if (s_title_countdown <= 0.0 && s_elapsed > 0.0)
        {
            char tmp[256];
            sprintf(tmp,
                MSG_TITLE MSG_TITLE_DELIMITER "%i FPS (%.4lfms/frame)",
                GetFPS(),
                GetFrameTime());
            SetWindowTitle(tmp);
            s_title_countdown = S_TITLE_COUNTDOWN_INITIAL;
        }
        // shared info
        const Vector2 mouse_pos = GetMousePosition();
        // input
        {
            // handle is mouse button clicked
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                is_spawning_monkeys = true;
            if (is_spawning_monkeys && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                is_spawning_monkeys = false;
            // change display ratio using mousehweel
            const int mouse_wheel = (int)GetMouseWheelMove();
            if (mouse_wheel != 0)
            {
                const Vector2 before = GetScreenToWorld2D(mouse_pos, cam);
                cam.zoom *= (mouse_wheel > 0) ? 1.11111f : 0.9f;
                if (cam.zoom < 1.0f)
                {
                    cam.zoom   = 1.0f;
                    cam.target = (Vector2){0,0};
                }
                else
                {
                    const Vector2 after  = GetScreenToWorld2D(mouse_pos, cam);
                    cam.target.x += before.x - after.x;
                    cam.target.y += before.y - after.y;
                }
            }
            // WASD movement
            if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) cam.target.x += CAM_MOVEMENT_STEP / cam.zoom;
            if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  cam.target.x -= CAM_MOVEMENT_STEP / cam.zoom;
            if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))  cam.target.y += CAM_MOVEMENT_STEP / cam.zoom;
            if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))    cam.target.y -= CAM_MOVEMENT_STEP / cam.zoom;
        }
        // tick
        {
            if (is_spawning_monkeys)
            {
                Monkey* monkey = world_monkeys_spawn_newborn(world);
                if (monkey != NULL)
                {
                    const Vector2 world_pos = GetScreenToWorld2D(mouse_pos, cam);
                    monkey->pos_x = world_pos.x;
                    monkey->pos_y = world_pos.y;
                    // set genes
                    monkey->dna.move_probability = (float)rand()/RAND_MAX;
                    monkey->dna.sight_radius     = MONKEY_SIGHT_RADIUS_DEFAULT;
                    printf("Spawned monkey #%llu at (%i, %i)\n", monkey->id, monkey->pos_x, monkey->pos_y);
                }
            }
            world_tick(world, WIDTH, HEIGHT, &count_death, &count_newborn, &dna_avg);
        }
        // draw
        BeginDrawing();
        {
            ClearBackground(BLACK);
            BeginMode2D(cam);
            {
                // draw terrain
                DrawTextureRec(
                    terrain_rt.texture,
                    (Rectangle){0, 0, WIDTH, -HEIGHT},
                    (Vector2){0,0},
                    WHITE
                );
                // draw monkeys
                for (size_t i=0; i<world->monkeys.size; ++i)
                {
                    Monkey* monkey = &world->monkeys.data[i];
                    const int pos_x = monkey->pos_x;
                    const int pos_y = monkey->pos_y;
                    if (monkey->age < MONKEY_AGE_ADULT)
                    {
                        DrawPixel(pos_x, pos_y-1, GetColor(0xF5DAB5FF));
                        DrawPixel(pos_x, pos_y,   GetColor(0x8C0B0BFF));
                    }
                    else if (monkey->age < MONKEY_AGE_OLD)
                    {
                        DrawPixel(pos_x, pos_y-2, GetColor(0xF5DAB5FF));
                        DrawPixel(pos_x, pos_y-1, GetColor(0x8C0B0BFF));
                        DrawPixel(pos_x, pos_y,   GetColor(0x8C0B0BFF));
                    }
                    else
                    {
                        DrawPixel(pos_x, pos_y-2, GetColor(0xF5DAB5FF));
                        DrawPixel(pos_x, pos_y-1, GetColor(0x915353FF));
                        DrawPixel(pos_x, pos_y,   GetColor(0x915353FF));
                    }
                }
            }
            EndMode2D();
            // draw ui
            {
                const bool is_showing_minimap = cam.zoom != CAM_ZOOM_DEFAULT;
                // TOP LEFT: draw minimap
                if (is_showing_minimap)
                {
                    int y_curr = 0;
                    y_curr += FONT_SIZE_EMPTYLINE;
                    const int minimap_pos_x  = FONT_SIZE_DEBUG;
                    const int minimap_pos_y  = y_curr;
                    const int minimap_width  = (float)WIDTH * MINIMAP_SCALE;
                    const int minimap_height = (float)HEIGHT * MINIMAP_SCALE;
                    // backdrop
                    DrawRectangle(
                        minimap_pos_x-2,
                        minimap_pos_y-2,
                        minimap_width+4,
                        minimap_height+4,
                        BLACK);
                    // minimap
                    DrawTexturePro(
                        terrain_rt.texture,
                        (Rectangle){
                            0, 0,
                            WIDTH, -HEIGHT
                        },
                        (Rectangle){
                            0, 0,
                            minimap_width, minimap_height
                        },
                        (Vector2){
                            - minimap_pos_x,
                            - minimap_pos_y
                        },
                        0.0f,
                        WHITE
                    );
                    y_curr += minimap_height;
                    // guide
                    if (cam.zoom > CAM_ZOOM_DEFAULT)
                    {
                        DrawRectangleLinesEx(
                            (Rectangle){
                                minimap_pos_x + cam.target.x * MINIMAP_SCALE,
                                minimap_pos_y + cam.target.y * MINIMAP_SCALE,
                                (minimap_width / cam.zoom),
                                (minimap_height / cam.zoom)
                        }, 2.0f, RED);
                    }
                    // ratio
                    char str_ratio[32];
                    sprintf(str_ratio, "Zoom: %.2f", cam.zoom);
                    y_curr += FONT_SIZE_EMPTYLINE,
                    DRAW_TEXT_DEBUG(str_ratio, FONT_SIZE_DEBUG, y_curr);
                    y_curr += FONT_SIZE_DEBUG;
                }
                // BOTTOM LEFT: draw debug
                const Vector2 world_pos = GetScreenToWorld2D(mouse_pos, cam);
                char str_mouse[32];
                sprintf(str_mouse, "(%i,%i)", (int)world_pos.x, (int)world_pos.y);
                int y_curr = HEIGHT;
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_mouse, FONT_SIZE_DEBUG, y_curr);
                char str_newborn[64];
                sprintf(str_newborn, "Newborn: %llu", count_newborn);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_newborn, FONT_SIZE_DEBUG, y_curr);
                char str_dead[64];
                sprintf(str_dead, "Dead: %llu", count_death);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_dead, FONT_SIZE_DEBUG, y_curr);
                char str_alive[64];
                sprintf(str_alive, "Alive: %zu", world->monkeys.size);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_alive, FONT_SIZE_DEBUG, y_curr);
                // BOTTOM RIGHT
                const int x_debug = WIDTH - 12*FONT_SIZE_DEBUG;
                y_curr = HEIGHT;
                y_curr -= FONT_SIZE_EMPTYLINE;
                char str_dna_score_water[32];
                sprintf(str_dna_score_water, "water score: %.2f", dna_avg.score_water);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_dna_score_water, x_debug, y_curr);
                char str_dna_sight_radius[32];
                sprintf(str_dna_sight_radius, "sight rad.: %.2f", dna_avg.sight_radius);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_dna_sight_radius, x_debug, y_curr);
                char str_dna_move_probability[32];
                sprintf(str_dna_move_probability, "move prob.: %.2f", dna_avg.move_probability);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_dna_move_probability, x_debug, y_curr);
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG("DNA average", x_debug, y_curr);
            }
        }
        EndDrawing();
    }
    // clean up
    CloseWindow();
    world_free(world);
    return 0;
}

