#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#include "raylib.h"

#include "terrain.h"
#include "noise.h"

#define MSG_TITLE "Cells"
#define MSG_TITLE_DELIMITER " | "

#define S_TITLE_COUNTDOWN_INITIAL 0.1

int main()
{
    // init
    const int width  = 1600;
    const int height = 900;
    InitWindow(width, height, MSG_TITLE);
    SetTargetFPS(60);
    srand(time(NULL));
    // data
    Terrain* terrain = terrain_new(width, height);
    terrain_perlin(terrain, width, height, 0.0001f, 0xA55FACE ^ 0xC0CB10C);
    // start loop
    double s_prev            = GetTime();
    double s_title_countdown = S_TITLE_COUNTDOWN_INITIAL;
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
        // draw
        BeginDrawing();
        {
            ClearBackground(BLACK);
            for (int i=0; i<height; ++i)
            {
                for (int j=0; j<width; ++j)
                {
#if 1
                    Color col = BLACK;
                    switch (terrain[i*width + j])
                    {
                        case TERRAIN_EARTH: col = DARKGREEN; break;
                        case TERRAIN_WATER: col = BLUE;      break;
                        default:            col = RED;       break;
                    }
                    DrawPixel(j, i, col);
#else
                    const double perlin = noise_perlin(j, i, 0xA55FACE);
                    DrawPixel(j, i, CLITERAL(Color){
                        (int)(255.0*perlin),
                        (int)(255.0*perlin),
                        (int)(255.0*perlin),
                        255,
                    });
#endif
                }
            }
        }
        EndDrawing();
    }
    // clean up
    CloseWindow();
    free(terrain);
    return 0;
}

