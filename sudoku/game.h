#pragma once

#include <raylib.h>
#include "board.h"
#include "layout.h"
#include "game_types.h"

#define TOTAL_TILES 81
#define DIFFICULTIES 3

typedef struct
{
  bool isHovered;
  bool isEnabled;
  Color color;
  Vector2 top_left;
  Vector2 bottom_right;
  int fontSize;
  char *label;
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
  Position hoveredTile;
  Stack *undoStack;
  Stack *redoStack;
  NumPadButton numPad[NUM_PAD_TILES][NUM_PAD_TILES];
  Button undoButton;
  Button redoButton;
  Button clearCellButton;
  Button newGameButton;
  Button gameOverButton;
  Button pauseButton;
  Button difficultyModeButton;
  Button difficultyButtons[DIFFICULTIES];
  RenderLayout layout;
  Time time;
  int errorCount;
  int visibleTilesCount;
  int maximumErrorsAllowed;
  bool isGameOver;
  bool isGameCompleted;
  bool isGamePaused;
  int difficultyIndex;
  Difficulty difficulties[DIFFICULTIES];
} Game;

void initDifficulty(Game *game);
void initRandomSeed();
void generateNewGame(Game *game);
void gameInit(Game *game, DifficultyIndex difficultyIndex);
void gameUnload(Game *game);
void moveStacks(Game *game, Stack **stack1, Stack **stack2, PossibleMoves move);
void undo(Game *game, Stack **undo_stack, Stack **redo_stack);
void redo(Game *game, Stack **undo_stack, Stack **redo_stack);
void clearCell(Game *game);
void newGame(Game *game, DifficultyIndex difficultyIndex);
void setPreviousValueNotCompleted(Game *game, int previousValue);
void clearNumPadValueCompleted(Game *game);
void isNumPadValueCompleted(Game *game, int digit);
bool checkErrors(Game *game);
void checkGameCompleted(Game *game);