#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "raylib.h"

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
            ClearBackground(BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

