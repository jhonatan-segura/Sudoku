#include <raylib.h>
#include <stdlib.h>
#include "render.h"
#include "game.h"
#include "input.h"

// TODO: Add isValid state when pushing or removing elements
// TODO: Add isCellCompleted state when pushing or removing elements
int main(void)
{
  Game *game = calloc(1, sizeof(Game));
  gameInit(game, EASY);

  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sudoku");

  SetTargetFPS(60);

  while (!WindowShouldClose())
  {
    // Update
    handleInput(game);

    // Draw
    BeginDrawing();

    ClearBackground(RAYWHITE);

    drawGame(game);

    EndDrawing();
  }

  // De-Initialization
  gameUnload(game);
  CloseWindow(); // Close window and OpenGL context

  return 0;
}