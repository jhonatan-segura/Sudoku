#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include "stdio.h"
#include "render.h"
#include "input.h"
#include "stack.h"

void initRandomSeed()
{
  srand(time(NULL));
}

void initRenderLayout(RenderLayout *layout)
{
  layout->boardSize = 0.6f;

  int boardEndFull = WINDOW_WIDTH * layout->boardSize;
  layout->boardEnd = boardEndFull - PADDING;

  layout->hudSize = (Vector2){
      boardEndFull + PADDING,
      WINDOW_WIDTH - PADDING};

  int baseButtonSize = (layout->hudSize.y - layout->hudSize.x) / 3;
  int numPadSize = baseButtonSize - HUD_GAP;
  int actionSize = numPadSize / 2.0f;
  int newGameSize = baseButtonSize * 3.0f - HUD_GAP;
  layout->numpadButtonSize = (Vector2){numPadSize, numPadSize};
  layout->actionButtonSize = (Vector2){actionSize, actionSize};
  layout->newGameButtonSize = (Vector2){newGameSize, numPadSize - 30};

  layout->tileSize = (layout->boardEnd - PADDING) / (float)TILES;
  layout->halfTileSize = layout->tileSize / 2.0f;
  layout->halfNumTileSize = layout->numpadButtonSize.x / 2.0f;
  layout->halfActionButtonSize = layout->actionButtonSize.x / 2.0f;
}

void initNumPad(Game *game)
{
  for (int i = 0; i < NUM_PAD_TILES; i++)
  {
    for (int j = 0; j < NUM_PAD_TILES; j++)
    {
      game->numPad[i][j].isCompleted = false;
    }
  }
}

void gameInit(Game *game)
{
  // --- current tile ---
  game->currentTile.x = -1;
  game->currentTile.y = -1;

  // --- stacks ---
  game->undoStack = NULL;
  game->redoStack = NULL;

  // --- buttons ---
  game->undoButton = (Button){0};
  game->undoButton.label = "<";
  game->redoButton = (Button){0};
  game->redoButton.label = ">";
  game->clearCellButton = (Button){0};
  game->clearCellButton.label = "x";
  game->newGameButton = (Button){0};
  game->newGameButton.label = "New Game";
  game->undoButton.isHovered = false;
  game->redoButton.isHovered = false;
  game->clearCellButton.isHovered = false;
  game->newGameButton.isHovered = false;

  // Time
  game->time.minutes = 0;
  game->time.seconds = 0;

  initNumPad(game);
  initRenderLayout(&game->layout);
  generateNewGame(game);
}

void generateNewGame(Game *game)
{
  initRandomSeed();
  initBoard(game->board);
  solver(game->board);
  hideTiles(game->board, EASY);
  printBoard(game->board);
}

void gameUnload(Game *game)
{
  freeStack(game->undoStack);
  freeStack(game->redoStack);
  free(game);
}

void moveStacks(Game *game, Stack **stack1, Stack **stack2, PossibleMoves move)
{
  if (*stack1 == NULL)
  {
    return;
  }
  Action action = pop(stack1);

  switch (move)
  {
  case UNDO:
    game->board[(int)action.position.x][(int)action.position.y].value = action.oldValue;
    game->board[(int)action.position.x][(int)action.position.y].hidden = action.oldHidden;
    game->currentTile.x = action.position.x;
    game->currentTile.y = action.position.y;
    game->currentTile.isSet = true;
    break;
  case REDO:
    game->board[(int)action.position.x][(int)action.position.y].value = action.newValue;
    game->board[(int)action.position.x][(int)action.position.y].hidden = action.newHidden;
    game->currentTile.x = action.position.x;
    game->currentTile.y = action.position.y;
    game->currentTile.isSet = true;
    break;
  }

  push(stack2, action);
}

void undo(Game *game, Stack **undo_stack, Stack **redo_stack)
{
  moveStacks(game, undo_stack, redo_stack, UNDO);
}

void redo(Game *game, Stack **undo_stack, Stack **redo_stack)
{
  moveStacks(game, redo_stack, undo_stack, REDO);
}

void clearCell(Game *game)
{
  Tile *selectedTile = &game->board[game->currentTile.x][game->currentTile.y];
  if (game->currentTile.isSet && !selectedTile->fixed)
  {
    selectedTile->value = selectedTile->targetValue;
    selectedTile->hidden = true;
  }
}

void newGame(Game *game)
{
  gameInit(game);

  freeStack(game->undoStack);
  freeStack(game->redoStack);
}

void isPreviousValueCompleted(Game *game, int previousValue)
{
  Vec2i numPadPosition = getNumPadPosition(previousValue);

  game->numPad[numPadPosition.x][numPadPosition.y].isCompleted = isDigitCompleted(game, previousValue);
}

void isSelectedValueCompleted(Game *game)
{
  int currentValue = game->board[game->currentTile.x][game->currentTile.y].value;
  Vec2i numPadPosition = getNumPadPosition(currentValue);

  game->numPad[numPadPosition.x][numPadPosition.y].isCompleted = isDigitCompleted(game, currentValue);
}

bool isDigitCompleted(Game *game, int digit)
{
  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      Tile currentTile = game->board[i][j];
      if ((currentTile.value != digit && currentTile.targetValue == digit) ||
          (currentTile.hidden && currentTile.targetValue == digit))
      {
        return false;
      }
    }
  }
  return true;
}