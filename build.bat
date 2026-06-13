gcc ./src/main.c ^
    ./src/noise.c ^
    ./src/terrain.c ^
    ./src/cell.c ^
    ./src/world.c ^
    -Wall -Wextra -Wpedantic ^
    -IC:/raylib/raylib/src/ ^
    -LC:/raylib/raylib/src/ ^
    -lraylib ^
    -lopengl32 ^
    -lgdi32 ^
    -lwinmm ^
    -o ./dist/main

