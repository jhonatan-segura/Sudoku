#pragma once

#include "game.h"

void drawGame(Game *game);
void drawBoard(Game *game);
void drawActionButtons(Game *game);
void drawNumPad(Game *game);
void drawTimer(Game *game);
void drawErrors(Game *game);
void drawPopUp(Game *game, char* title, char* body, char* buttonText);