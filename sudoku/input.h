#pragma once

#include <raylib.h>
#include "game.h"

#define ALLOWED_NUMBERS 9

void handleInput(Game *game);
void isBoardPressed(Game *game, Vector2 mousePos);
void handleKeyboardDigits(Game *game);
void handlePause(Game *game);
void handleUnpauseGame(Game *game);
void isNumPadPressed(Game *game, Vector2 mousePos);
void handleMouse(Game *game, Vector2 mousePos);
bool isTileHovered(Game *game, int x, int y, Vector2 mousePos);