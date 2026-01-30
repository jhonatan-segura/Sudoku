#pragma once

#include <raylib.h>
#include "board.h"
#include "layout.h"
#include "game_types.h"

#define EASY 40
#define MEDIUM 50
#define HARD 60

typedef struct
{
  bool isHovered;
  Color color;
  Vector2 top_left;
  Vector2 bottom_right;
} Button;

typedef struct
{
  int value;
  bool isHovered;
  bool isCompleted;
  Color color;
  Vector2 top_left;
  Vector2 bottom_right;
} NumPadButton;
typedef enum
{
  UNDO,
  REDO
} PossibleMoves;

typedef struct
{
  int minutes;
  int seconds;
} Time;
typedef struct
{
  Tile board[TILES][TILES];
  Position currentTile;
  Stack *undoStack;
  Stack *redoStack;
  NumPadButton numPad[NUM_PAD_TILES][NUM_PAD_TILES];
  Button undoButton;
  Button redoButton;
  Button clearCellButton;
  Button newGameButton;
  RenderLayout layout;
  Time time;
} Game;

void initRandomSeed();
void generateNewGame(Game *game);
void gameInit(Game *game);
void gameUnload(Game *game);
void moveStacks(Game *game, Stack **stack1, Stack **stack2, PossibleMoves move);
void undo(Game *game, Stack **undo_stack, Stack **redo_stack);
void redo(Game *game, Stack **undo_stack, Stack **redo_stack);
void clearCell(Game *game);
void newGame(Game *game);
void isPreviousValueCompleted(Game *game, int previousValue);
void isSelectedValueCompleted(Game *game);
bool isDigitCompleted(Game *game, int digit);