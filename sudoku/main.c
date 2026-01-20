#include <raylib.h>
#include "render.h"
#include "game.h"
#include "input.h"

int main(void)
{
  Game game;
  gameInit(&game);

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sudoku");

  SetTargetFPS(60);

  // Main game loop
  while (!WindowShouldClose())
  {
    // Update
    handleInput(&game);

    // Draw
    BeginDrawing();

    ClearBackground(RAYWHITE);

    drawGame(&game);

    EndDrawing();
  }

  // De-Initialization
  gameUnload(&game);
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context

  return 0;
}