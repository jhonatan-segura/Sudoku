#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include "stdio.h"
#include "render.h"
#include "input.h"
#include "stack.h"

void initTime()
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

  int buttonSize = (layout->hudSize.y - layout->hudSize.x) / 3 - HUD_GAP;
  layout->numpadButtonSize = (Vector2){buttonSize, buttonSize};
  layout->actionButtonSize = (Vector2){buttonSize / 2.0f, buttonSize / 2.0f};

  layout->tileSize = (layout->boardEnd - PADDING) / (float)TILES;
  layout->halfTileSize = layout->tileSize / 2.0f;
  layout->halfNumTileSize = layout->numpadButtonSize.x / 2.0f;
  layout->halfActionButtonSize = layout->actionButtonSize.x / 2.0f;
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
  game->redoButton = (Button){0};
  game->clearCellButton = (Button){0};
  game->undoButton.selected = false;
  game->redoButton.selected = false;
  game->clearCellButton.selected = false;

  initRenderLayout(&game->layout);
  initTime();
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