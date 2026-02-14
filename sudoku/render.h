#pragma once

#include "game.h"
#define TRANSLUCID_BACKGROUND (Color){0, 0, 0, 150}
#define LIGHT_BLUE (Color){121, 172, 224, 255}
#define FONT_SIZE_XS 20
#define FONT_SIZE_S 26
#define FONT_SIZE_M 28
#define FONT_SIZE_L 35
#define FONT_SIZE_XL 40
#define LINE_THICKNESS_M 3.0
#define LINE_THICKNESS_S 2.0

void drawGame(Game *game);
void drawBoard(Game *game);
void drawActionButtons(Game *game);
void drawNumPad(Game *game);
void drawTimer(Game *game);
void drawErrors(Game *game);
void drawPopUp(Game *game, char *title, char *body, char *buttonText);
void drawPauseScreen();
void drawPauseBars(int width, int height, int gap, int xPos, int yPos, Color color);
void drawTileOutline(int x, int y, int tileSize, Color color);
void drawDifficultyPopUp(Game *game);
void drawBackgroundSkeleton(int screenWidth, int screenHeight);