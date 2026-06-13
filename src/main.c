#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "raylib.h"

#include "world.h"

#define MSG_TITLE "Cells"
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
                #define _CASE(c,r,g,b) case c: col = (Color){r,g,b,255}; break
                _CASE(TERRAIN_EARTH,         0x7C,0xBF,0x58);
                _CASE(TERRAIN_STONE,         0x7A,0x6D,0x6D);
                _CASE(TERRAIN_WATER,         0xA9,0xE2,0xF5);
                _CASE(TERRAIN_WATER_SHALLOW, 0xB9,0xEB,0xFA);
                _CASE(TERRAIN_WATER_DEEP,    0x98,0xD6,0xEB);
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
            // cell on click
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                Cell cell;
                const Vector2 world_pos = GetScreenToWorld2D(mouse_pos, cam);
                cell.pos_x = world_pos.x;
                cell.pos_y = world_pos.y;
                dyn_array_cell_append(&world->cells, cell);
                printf("added cell at (%i, %i)\n", cell.pos_x, cell.pos_y);
            }
            // change display ratio using mousehweel
            const int mouse_wheel = (int)GetMouseWheelMove();
            if (mouse_wheel != 0)
            {
                const Vector2 before = GetScreenToWorld2D(mouse_pos, cam);
                cam.zoom *= (mouse_wheel > 0) ? 2.0f : 0.5f;
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
            if (IsKeyDown(KEY_D)) cam.target.x += CAM_MOVEMENT_STEP / cam.zoom;
            if (IsKeyDown(KEY_A)) cam.target.x -= CAM_MOVEMENT_STEP / cam.zoom;
            if (IsKeyDown(KEY_S)) cam.target.y += CAM_MOVEMENT_STEP / cam.zoom;
            if (IsKeyDown(KEY_W)) cam.target.y -= CAM_MOVEMENT_STEP / cam.zoom;
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
                // draw cells
                for (size_t i=0; i<world->cells.size; ++i)
                {
                    const int pos_x = world->cells.data[i].pos_x;
                    const int pos_y = world->cells.data[i].pos_y;
                    DrawPixel(pos_x, pos_y-2, (Color){0xF5,0xDA,0xB5,0xFF});
                    DrawPixel(pos_x, pos_y-1, (Color){0x8C,0x0B,0x0B,0xFF});
                    DrawPixel(pos_x, pos_y,   (Color){0x8C,0x0B,0x0B,0xFF});
                }
            }
            EndMode2D();
            // draw ui
            {
                const bool is_showing_minimap = cam.zoom != CAM_ZOOM_DEFAULT;
                // draw minimap
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
                // draw debug from the bottom
                const Vector2 world_pos = GetScreenToWorld2D(mouse_pos, cam);
                char str_mouse[32];
                sprintf(str_mouse, "(%i,%i)", (int)world_pos.x, (int)world_pos.y);
                int y_curr = HEIGHT;
                y_curr -= FONT_SIZE_EMPTYLINE;
                y_curr -= FONT_SIZE_DEBUG;
                DRAW_TEXT_DEBUG(str_mouse, FONT_SIZE_DEBUG, y_curr);
                y_curr -= FONT_SIZE_DEBUG;
            }
        }
        EndDrawing();
    }
    // clean up
    CloseWindow();
    world_free(world);
    return 0;
}

