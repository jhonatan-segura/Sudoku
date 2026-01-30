#include "render.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

float prevTime = 0.0f;
float now = 0.0f;

void drawGame(Game *game)
{
  drawBoard(game);
  drawNumPad(game);
  drawActionButtons(game);
  drawTimer(game);
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
      Tile *currentTile = &game->board[i][j];
      int x1 = j * tileSize + PADDING;
      int y1 = i * tileSize + PADDING;
      int x2 = (j + 1) * tileSize + PADDING;
      int y2 = (i + 1) * tileSize + PADDING;
      currentTile->top_left = (Vec2){x1, y1};
      currentTile->bottom_right = (Vec2){x2, y2};

      if (currentTile->value == selectedCell.value && !selectedCell.hidden && !currentTile->hidden)
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

      if (currentTile->hidden)
      {
        continue;
      }

      int x_coord = x1 + halfTileSize - 5;
      int y_coord = y1 + TEXT_PADDING;
      const char *cellText = TextFormat("%i", currentTile->value);
      Color textColor = currentTile->fixed ? BLACK : GRAY;
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

void drawButton(Vec2 buttonPosition, Vector2 buttonSize, Vec2 textPosition, Button *button)
{
  int newGameX = buttonPosition.x;
  int newGameY = buttonPosition.y;

  Rectangle newGameRect = {
      .x = newGameX,
      .y = newGameY,
      .width = buttonSize.x,
      .height = buttonSize.y};

  button->top_left = (Vector2){newGameX, newGameY};
  button->bottom_right = (Vector2){newGameX + buttonSize.x, newGameY + buttonSize.y};
  Vector2 newGamePos = (Vector2){newGameRect.x, newGameRect.y};
  Vector2 newGameSize = (Vector2){newGameRect.width, newGameRect.height};
  button->color = button->isHovered ? LIGHTGRAY : WHITE;
  DrawRectangleV(newGamePos, newGameSize, button->color);

  DrawRectangleLinesEx(newGameRect, 2.0, BLACK);
  DrawText(button->label, newGameX + textPosition.x, newGameY + textPosition.y, 35, BLACK);
}

void drawActionButtons(Game *game)
{
  Vector2 numpadButtonSize = game->layout.numpadButtonSize;
  Vector2 actionButtonSize = game->layout.actionButtonSize;
  Vector2 newGameButtonSize = game->layout.newGameButtonSize;
  Vector2 hudSize = game->layout.hudSize;
  float halfActionSize = game->layout.halfActionButtonSize;

  int undoX = hudSize.x;
  int undoY = PADDING + numpadButtonSize.x + HUD_GAP;
  drawButton((Vec2){undoX, undoY}, actionButtonSize, (Vec2){halfActionSize - 5, halfActionSize - 14}, &game->undoButton);

  int redoX = hudSize.x + actionButtonSize.x + HUD_GAP;
  int redoY = PADDING + numpadButtonSize.x + HUD_GAP;
  drawButton((Vec2){redoX, redoY}, actionButtonSize, (Vec2){halfActionSize - 5, halfActionSize - 14}, &game->redoButton);

  int clearX = hudSize.x + (actionButtonSize.x + HUD_GAP) * 2;
  int clearY = PADDING + numpadButtonSize.x + HUD_GAP;
  drawButton((Vec2){clearX, clearY}, actionButtonSize, (Vec2){halfActionSize - 5, halfActionSize - 14}, &game->clearCellButton);

  drawButton((Vec2){hudSize.x, PADDING}, newGameButtonSize, (Vec2){halfActionSize - 5, halfActionSize - 14}, &game->newGameButton);
}

void drawNumPad(Game *game)
{
  Vector2 numpadButtonSize = game->layout.numpadButtonSize;
  Vector2 hudSize = game->layout.hudSize;
  float halfNumTileSize = game->layout.halfNumTileSize;

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

      NumPadButton *currentButton = &game->numPad[i][j];
      currentButton->top_left = (Vector2){x1, y1};
      currentButton->bottom_right = (Vector2){x2, y2};
      currentButton->value = numpad_count;
      if (currentButton->isCompleted)
      {
        currentButton->color = GREEN;
      }
      else if (currentButton->isHovered)
      {
        currentButton->color = LIGHTGRAY;
      }
      else
      {
        currentButton->color = WHITE;
      }

      Rectangle rect = {
          .x = x1,
          .y = y1,
          .width = numpadButtonSize.x,
          .height = numpadButtonSize.y};
      Vector2 rectPos = (Vector2){rect.x, rect.y};
      Vector2 rectSize = (Vector2){rect.width, rect.height};
      DrawRectangleV(rectPos, rectSize, currentButton->color);
      DrawRectangleLinesEx(rect, 2.0, BLACK);

      const char *cellText = TextFormat("%i", numpad_count++);
      int num_x = x1 + halfNumTileSize - 5;
      int num_y = y1 + halfNumTileSize - 14;
      DrawText(cellText, num_x, num_y, 28, BLACK);
    }
  }
}

void drawTimer(Game *game)
{
  now = GetTime();
  if (now - prevTime >= 1.0f)
  {
    prevTime = now;
    game->time.seconds++;
  }
  if (game->time.seconds == 60)
  {
    game->time.minutes++;
    game->time.seconds = 0.0;
  }

  const char *time = TextFormat("%02d:%02d", game->time.minutes, game->time.seconds);
  DrawText(time, PADDING, 10, 28, BLACK);
}