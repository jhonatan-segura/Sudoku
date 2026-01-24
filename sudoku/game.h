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
  bool selected;
  Color color;
  Vector2 top_left;
  Vector2 bottom_right;
} Button;

typedef struct
{
  int value;
  bool selected;
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
  Tile board[TILES][TILES];
  Position currentTile;
  Stack *undoStack;
  Stack *redoStack;
  NumPadButton numPad[NUM_PAD_TILES][NUM_PAD_TILES];
  Button undoButton;
  Button redoButton;
  Button clearCellButton;
  RenderLayout layout;
} Game;

void initTime();
void gameInit(Game *game);
void gameUnload(Game *game);
void moveStacks(Game *game, Stack **stack1, Stack **stack2, PossibleMoves move);
void undo(Game *game, Stack **undo_stack, Stack **redo_stack);
void redo(Game *game, Stack **undo_stack, Stack **redo_stack);
void clearCell(Game *game);