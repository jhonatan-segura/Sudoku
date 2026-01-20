#pragma once

#include "utils.h"
#include <stdbool.h>

#define TILES 9
#define NUM_PAD_TILES 3
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 700
#define PADDING 40
#define HUD_GAP 20
#define TEXT_PADDING 18
#define LINE_THICKNESS 3.0

typedef struct
{
  int value;
  int targetValue;
  int attempts[TILES];
  bool hidden;
  bool fixed;
  Vec2 top_left;
  Vec2 bottom_right;
} Tile;

typedef struct
{
  int x;
  int y;
  bool isSet;
  int tempValue;
  int attempts[TILES];
} Position;

void initTime();
bool isValidCol(Tile board[][TILES], Position *currentPos);
bool isValidRow(Tile board[][TILES], Position *currentPos);
bool notAttemptedYet(Tile board[][TILES], Position *currentPos);
bool fillCell(Tile board[][TILES], Position *currentPos);
void resetAttempts(Tile board[][TILES], Position *currentPos);
bool isRandomValueValid(Tile board[][TILES], Position *currentPos);
void goBack(Position *currentPos);
bool goForth(Position *currentPos);

void solver(Tile board[][TILES]);
void initBoard(Tile board[][TILES]);
void hideTiles(Tile board[][TILES], int numTiles);
void printBoard(Tile board[][TILES]);