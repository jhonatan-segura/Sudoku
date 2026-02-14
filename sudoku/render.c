#include "render.h"
#include "game.h"
#include "utils.h"
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
  drawErrors(game);

  if (game->isGameOver)
    drawPopUp(game, "Game Over", "You've made 3 mistakes!", "Try Again");

  if (game->isGameCompleted)
    drawPopUp(game, "Sudoku Solved!", "You've completed the sudoku!", "New Game");

  if (game->isGamePaused)
    drawPauseScreen();

  if (game->difficultyModeButton.isEnabled)
    drawDifficultyPopUp(game);
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
      DrawLineEx(start_hor, end_hor, LINE_THICKNESS_M, BLACK);
      DrawLineEx(start_ver, end_ver, LINE_THICKNESS_M, BLACK);
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
        drawTileOutline(x1, y1, tileSize, LIGHT_BLUE);
      }

      if (game->hoveredTile.isSet && game->hoveredTile.x == i && game->hoveredTile.y == j)
      {
        Color tileInnerColor = GRAY;
        drawTileOutline(x1, y1, tileSize, tileInnerColor);
      }

      if (game->currentTile.x == i && game->currentTile.y == j)
      {
        Color tileInnerColor = currentTile->isValid ? BLUE : RED;
        drawTileOutline(x1, y1, tileSize, tileInnerColor);
      }

      if (currentTile->hidden)
      {
        continue;
      }

      int x_coord = x1 + halfTileSize - 5;
      int y_coord = y1 + TEXT_PADDING;
      const char *cellText = TextFormat("%i", currentTile->value);
      Color textColor = BLACK;

      if (currentTile->fixed)
        textColor = BLACK;
      else if (!currentTile->isValid)
        textColor = RED;
      else
        textColor = BLUE;

      DrawText(cellText, x_coord, y_coord, FONT_SIZE_M, textColor);
    }
  }
}

void drawTileOutline(int x, int y, int tileSize, Color color)
{
  Rectangle rect = {
      .x = x,
      .y = y,
      .width = tileSize,
      .height = tileSize};
  DrawRectangleLinesEx(rect, LINE_THICKNESS_M, color);
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

  DrawRectangleLinesEx(newGameRect, LINE_THICKNESS_S, BLACK);
  DrawText(button->label, newGameX + textPosition.x, newGameY + textPosition.y, button->fontSize, BLACK);
}

void drawActionButtons(Game *game)
{
  Vector2 numpadButtonSize = game->layout.numpadButtonSize;
  Vector2 actionButtonSize = game->layout.actionButtonSize;
  Vector2 newGameButtonSize = game->layout.newGameButtonSize;
  Vector2 pauseButtonSize = game->layout.pauseButtonSize;
  Vector2 difficultyButtonSize = game->layout.difficultyButtonSize;
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

  drawButton((Vec2){PADDING + 100, 8}, pauseButtonSize, (Vec2){7, 5}, &game->pauseButton);
  drawPauseBars(3, 15, 5, PADDING + 112, 14, BLACK);

  drawButton((Vec2){PADDING + 200, 8}, difficultyButtonSize, (Vec2){7, 5}, &game->difficultyModeButton);
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
        currentButton->color = GREEN;
      else if (currentButton->isHovered)
        currentButton->color = LIGHTGRAY;
      else
        currentButton->color = WHITE;

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
      DrawText(cellText, num_x, num_y, FONT_SIZE_M, BLACK);
    }
  }
}

void drawTimer(Game *game)
{
  if (!game->isGameOver && !game->isGameCompleted && !game->isGamePaused)
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
  }

  const char *time = TextFormat("%02d:%02d", game->time.minutes, game->time.seconds);
  DrawText(time, PADDING, 10, FONT_SIZE_M, BLACK);
}

void drawErrors(Game *game)
{
  const char *errorText = TextFormat("Mistakes: %d/%d", game->errorCount, game->maximumErrorsAllowed);
  const int textXPos = MeasureText(errorText, 28);
  DrawText(errorText, game->layout.boardEnd - textXPos, 10, FONT_SIZE_M, BLACK);
}

void drawPopUp(Game *game, char *title, char *body, char *buttonText)
{
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  drawBackgroundSkeleton(screenWidth, screenHeight);

  // Draw button
  const int buttonVerticalDisplacement = 70;
  const int buttonWidth = 200;
  const int buttonHeight = 30;
  const int buttonX = (screenWidth / 2) - (buttonWidth / 2);
  const int buttonY = (screenHeight / 2) + buttonVerticalDisplacement;
  Rectangle buttonPosition = (Rectangle){
      game->gameOverButton.top_left.x,
      game->gameOverButton.top_left.y,
      buttonWidth,
      buttonHeight};
  game->gameOverButton.top_left = (Vector2){buttonX, buttonY};
  game->gameOverButton.bottom_right = (Vector2){buttonWidth + buttonX, buttonHeight + buttonY};
  Color buttonColor = game->gameOverButton.isHovered ? LIGHTGRAY : RAYWHITE;
  DrawRectangleRounded(buttonPosition, 0.7, 30, buttonColor);
  DrawRectangleRoundedLinesEx(buttonPosition, 0.7, 30, LINE_THICKNESS_M, BLACK);

  const int titleX = getTextPosition(screenWidth, title, FONT_SIZE_M);
  const int titleY = (screenHeight / 2) - 90;

  const int bodyX = getTextPosition(screenWidth, body, FONT_SIZE_S);
  const int bodyY = (screenHeight / 2) - 10;

  const int buttonTextX = getTextPosition(screenWidth, buttonText, FONT_SIZE_S);
  const int buttonTextY = game->gameOverButton.top_left.y;

  DrawText(title, titleX, titleY, FONT_SIZE_M, BLACK);
  DrawText(body, bodyX, bodyY, FONT_SIZE_S, BLACK);
  DrawText(buttonText, buttonTextX, buttonTextY, FONT_SIZE_S, BLACK);
}

void drawDifficultyPopUp(Game *game)
{
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  drawBackgroundSkeleton(screenWidth, screenHeight);

  const int buttonTitleX = getTextPosition(screenWidth, "Choose the game difficulty:", FONT_SIZE_S);
  const int buttonTitleY = (screenHeight / 2) - 100;

  DrawText("Choose the game difficulty:", buttonTitleX, buttonTitleY, FONT_SIZE_S, BLACK);

  // Draw buttons
  for (int i = 0; i < DIFFICULTIES; i++)
  {
    const int buttonVerticalDisplacement = 40 * i;
    const int buttonWidth = 200;
    const int buttonHeight = 30;
    const int buttonX = (screenWidth / 2) - (buttonWidth / 2);
    const int buttonY = (screenHeight / 2) + buttonVerticalDisplacement;
    game->difficultyButtons[i].top_left = (Vector2){buttonX, buttonY};
    Rectangle buttonPosition = (Rectangle){
        game->difficultyButtons[i].top_left.x,
        game->difficultyButtons[i].top_left.y,
        buttonWidth,
        buttonHeight};
    game->difficultyButtons[i].bottom_right = (Vector2){buttonWidth + buttonX, buttonHeight + buttonY};
    Color buttonColor = game->difficultyButtons[i].isHovered ? LIGHTGRAY : RAYWHITE;
    DrawRectangleRounded(buttonPosition, 0.7, 30, buttonColor);
    DrawRectangleRoundedLinesEx(buttonPosition, 0.7, 30, LINE_THICKNESS_M, BLACK);

    const int buttonEasyX = getTextPosition(screenWidth, game->difficulties[i].text, FONT_SIZE_S);
    const int buttonEasyY = (screenHeight / 2) + buttonVerticalDisplacement;

    DrawText(game->difficulties[i].text, buttonEasyX, buttonEasyY, FONT_SIZE_S, BLACK);
  }
}

void drawPauseScreen()
{
  const int screenWidth = GetScreenWidth();
  const int screenHeight = GetScreenHeight();

  DrawRectangle(0, 0, screenWidth, screenHeight, TRANSLUCID_BACKGROUND);

  drawPauseBars(40, 150, 30,
                (screenWidth / 2) - (40 / 2),
                (screenHeight / 2) - (150 / 2),
                RAYWHITE);

  const int buttonTextX = getTextPosition(screenWidth, "Game Paused", FONT_SIZE_XL);
  const int buttonTextY = 150;

  DrawText("Game Paused", buttonTextX, buttonTextY, FONT_SIZE_XL, RAYWHITE);
}

void drawPauseBars(int width, int height, int gap, int xPos, int yPos, Color color)
{
  const int barWidth = width;
  const int barHeight = height;
  const int dividingGap = gap;
  const int leftRectX = xPos - dividingGap;
  const int leftRectY = yPos;
  Vector2 leftRectPosition = (Vector2){
      leftRectX,
      leftRectY};
  Vector2 leftRectSize = (Vector2){
      barWidth,
      barHeight};
  DrawRectangleV(leftRectPosition, leftRectSize, color);

  const int rightRectX = xPos + dividingGap;
  const int rightRectY = yPos;
  Vector2 rightRectPosition = (Vector2){
      rightRectX,
      rightRectY};
  Vector2 rightRectSize = (Vector2){
      barWidth,
      barHeight};
  DrawRectangleV(rightRectPosition, rightRectSize, color);
}

void drawBackgroundSkeleton(int screenWidth, int screenHeight)
{
  DrawRectangle(0, 0, screenWidth, screenHeight, TRANSLUCID_BACKGROUND);

  const int popUpWidth = 420;
  const int popUpHeigth = 250;
  const int backgroundX = (screenWidth / 2) - (popUpWidth / 2);
  const int backgroundY = (screenHeight / 2) - (popUpHeigth / 2);
  Rectangle backgroundPosition = (Rectangle){
      backgroundX,
      backgroundY,
      popUpWidth,
      popUpHeigth};
  DrawRectangleRounded(backgroundPosition, 0.1, 30, RAYWHITE);
  DrawRectangleRoundedLinesEx(backgroundPosition, 0.1, 30, LINE_THICKNESS_M, BLACK);
}