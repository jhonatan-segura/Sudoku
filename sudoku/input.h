#pragma once

#include <raylib.h>
#include "game.h"

#define ALLOWED_NUMBERS 9

void handleInput(Game *game);
void isBoardPressed(Game *game, Vector2 mousePos);
void handleKeyboard(Game *game);
void isNumPadPressed(Game *game, Vector2 mousePos);
void isActionClicked(Game *game, Vector2 mousePos);
bool isDigitCompleted(Game *game, int digit);