gcc ./src/main.c ^
    -Wall -Wextra -Wpedantic ^
    -IC:/raylib/raylib/src/ ^
    -LC:/raylib/raylib/src/ ^
    -lraylib ^
    -lopengl32 ^
    -lgdi32 ^
    -lwinmm ^
    -o ./dist/main

