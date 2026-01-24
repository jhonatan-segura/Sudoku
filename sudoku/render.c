#include "render.h"
#include "game.h"
#include <stdlib.h>

float prevTime = 0.0f;
float now = 0.0f;
int minutes = 0;
int seconds = 0;

void drawGame(Game *game)
{
  drawBoard(game);
  drawNumPad(game);
  drawActionButtons(game);
  drawTimer();
}

void drawBoardGrid(Game *game, float tileSize)
{
  for (int i = 0; i < TILES + 1; i++)
  {
    int coord = i * tileSize + PADDING;
    Vector2 start_hor = (Vector2){PADDING, coord};
    Vector2 end_hor = (Vector2){game->layout.boardEnd, coord};
    DrawLineV(start_hor, end_hor, BLACK);

    Vector2 start_ver = (Vector2){coord, PADDING};
    Vector2 end_ver = (Vector2){coord, game->layout.boardEnd};
    DrawLineV(start_ver, end_ver, BLACK);

    if (i % 3 == 0)
    {
      DrawLineEx(start_hor, end_hor, LINE_THICKNESS, BLACK);
      DrawLineEx(start_ver, end_ver, LINE_THICKNESS, BLACK);
    }
  }
}

void drawBoardDigits(Game *game, float tileSize, float halfTileSize)
{
  Tile selectedCell = game->board[game->currentTile.x][game->currentTile.y];

  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      int x1 = j * tileSize + PADDING;
      int y1 = i * tileSize + PADDING;
      int x2 = (j + 1) * tileSize + PADDING;
      int y2 = (i + 1) * tileSize + PADDING;
      game->board[i][j].top_left = (Vec2){x1, y1};
      game->board[i][j].bottom_right = (Vec2){x2, y2};

      if (game->board[i][j].value == selectedCell.value && !selectedCell.hidden && !game->board[i][j].hidden)
      {
        Rectangle rect = {
            .x = x1,
            .y = y1,
            .width = tileSize,
            .height = tileSize};
        DrawRectangleLinesEx(rect, 3.0, (Color){121, 172, 224, 255});
      }

      if (game->currentTile.x == i && game->currentTile.y == j)
      {
        Rectangle rect = {
            .x = x1,
            .y = y1,
            .width = tileSize,
            .height = tileSize};
        DrawRectangleLinesEx(rect, 3.0, BLUE);
      }

      if (game->board[i][j].hidden)
      {
        continue;
      }

      int x_coord = x1 + halfTileSize - 5;
      int y_coord = y1 + TEXT_PADDING;
      const char *cellText = TextFormat("%i", game->board[i][j].value);
      Color textColor = game->board[i][j].fixed ? BLACK : GRAY;
      DrawText(cellText, x_coord, y_coord, 28, textColor);
    }
  }
}

void drawBoard(Game *game)
{
  float tileSize = game->layout.tileSize;
  float halfTileSize = game->layout.halfTileSize;

  drawBoardGrid(game, tileSize);
  drawBoardDigits(game, tileSize, halfTileSize);
}

void drawActionButtons(Game *game)
{
  Vector2 numpadButtonSize = game->layout.numpadButtonSize;
  Vector2 actionButtonSize = game->layout.actionButtonSize;
  Vector2 hudSize = game->layout.hudSize;
  float halfActionSize = game->layout.halfActionButtonSize;

  // Draw undo/redo buttons
  int undo_x = hudSize.x;
  int undo_y = PADDING + numpadButtonSize.x + HUD_GAP;

  Rectangle undoRect = {
      .x = undo_x,
      .y = undo_y,
      .width = actionButtonSize.x,
      .height = actionButtonSize.y};

  game->undoButton.top_left = (Vector2){undo_x, undo_y};
  game->undoButton.bottom_right = (Vector2){undo_x + actionButtonSize.x, undo_y + actionButtonSize.y};
  Vector2 undoPos = (Vector2){undoRect.x, undoRect.y};
  Vector2 undoSize = (Vector2){undoRect.width, undoRect.height};
  game->undoButton.color = game->undoButton.selected ? LIGHTGRAY : WHITE;
  DrawRectangleV(undoPos, undoSize, game->undoButton.color);

  DrawRectangleLinesEx(undoRect, 2.0, BLACK);
  DrawText("<", undo_x + halfActionSize - 5, undo_y + halfActionSize - 14, 35, BLACK);

  // redo
  int redo_x = hudSize.x + actionButtonSize.x + HUD_GAP;
  int redo_y = PADDING + numpadButtonSize.x + HUD_GAP;

  Rectangle redoRect = {
      .x = redo_x,
      .y = redo_y,
      .width = actionButtonSize.x,
      .height = actionButtonSize.y};

  game->redoButton.top_left = (Vector2){redo_x, redo_y};
  game->redoButton.bottom_right = (Vector2){redo_x + actionButtonSize.x, redo_y + actionButtonSize.y};
  Vector2 redoPos = (Vector2){redoRect.x, redoRect.y};
  Vector2 redoSize = (Vector2){redoRect.width, redoRect.height};
  game->redoButton.color = game->redoButton.selected ? LIGHTGRAY : WHITE;
  DrawRectangleV(redoPos, redoSize, game->redoButton.color);

  DrawRectangleLinesEx(redoRect, 2.0, BLACK);
  DrawText(">", redo_x + halfActionSize - 5, redo_y + halfActionSize - 14, 35, BLACK);

  // clearButton
  int clear_x = hudSize.x + (actionButtonSize.x + HUD_GAP) * 2;
  int clear_y = PADDING + numpadButtonSize.x + HUD_GAP;

  Rectangle clearRect = {
      .x = clear_x,
      .y = clear_y,
      .width = actionButtonSize.x,
      .height = actionButtonSize.y};

  game->clearCellButton.top_left = (Vector2){clear_x, clear_y};
  game->clearCellButton.bottom_right = (Vector2){clear_x + actionButtonSize.x, clear_y + actionButtonSize.y};
  Vector2 clearPos = (Vector2){clearRect.x, clearRect.y};
  Vector2 clearSize = (Vector2){clearRect.width, clearRect.height};
  game->clearCellButton.color = game->clearCellButton.selected ? LIGHTGRAY : WHITE;
  DrawRectangleV(clearPos, clearSize, game->clearCellButton.color);

  DrawRectangleLinesEx(clearRect, 2.0, BLACK);
  DrawText("x", clear_x + halfActionSize - 5, clear_y + halfActionSize - 14, 35, BLACK);
}

void drawNumPad(Game *game)
{
  Vector2 numpadButtonSize = game->layout.numpadButtonSize;
  Vector2 hudSize = game->layout.hudSize;
  float halfNumTileSize = game->layout.halfNumTileSize;

  // Draw numpad
  int numpad_count = 1;
  for (int i = 0; i < NUM_PAD_TILES; i++)
  {
    for (int j = 0; j < NUM_PAD_TILES; j++)
    {
      // Coordinates to draw numpad buttons and store their positions to detect clicks.
      int x1 = j * (numpadButtonSize.x + HUD_GAP) + hudSize.x;
      int y1 = i * (numpadButtonSize.x + HUD_GAP) + 280;
      int x2 = x1 + numpadButtonSize.x;
      int y2 = y1 + numpadButtonSize.x;

      game->numPad[i][j].top_left = (Vector2){x1, y1};
      game->numPad[i][j].bottom_right = (Vector2){x2, y2};
      game->numPad[i][j].value = numpad_count;
      game->numPad[i][j].color = game->numPad[i][j].selected ? LIGHTGRAY : WHITE;

      Rectangle rect = {
          .x = x1,
          .y = y1,
          .width = numpadButtonSize.x,
          .height = numpadButtonSize.y};
      Vector2 rectPos = (Vector2){rect.x, rect.y};
      Vector2 rectSize = (Vector2){rect.width, rect.height};
      DrawRectangleV(rectPos, rectSize, game->numPad[i][j].color);
      DrawRectangleLinesEx(rect, 2.0, BLACK);

      const char *cellText = TextFormat("%i", numpad_count++);
      int num_x = x1 + halfNumTileSize - 5;
      int num_y = y1 + halfNumTileSize - 14;
      DrawText(cellText, num_x, num_y, 28, BLACK);
    }
  }
}

void drawTimer()
{
  now = GetTime();
  if (now - prevTime >= 1.0f)
  {
    prevTime = now;
    seconds++;
  }
  if (seconds == 60)
  {
    minutes++;
    seconds = 0.0;
  }

  const char *time = TextFormat("%02d:%02d", minutes, seconds);
  // printf("Time: %02d\n", minutes);
  DrawText(time, PADDING, 10, 28, BLACK);
}

void printValues(Stack *stack)
{
  while (stack != NULL)
  {
    // printf("%d\n", stack->action);
    stack = stack->next;
  }
}