#include "input.h"
#include "game.h"
#include "stdio.h"
#include "stack.h"

void handleInput(Game *game)
{
  Vector2 mousePos = GetMousePosition();
  // printf("Mouse over %f, %f\n", mousePos.x, mousePos.y);
  isNumPadPressed(game, mousePos);
  isActionClicked(game, mousePos);
  isBoardPressed(game, mousePos);
  handleKeyboard(game);
}

void handleKeyboard(Game *game)
{
  int numbers[ALLOWED_NUMBERS] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE};
  for (int i = 0; i < ALLOWED_NUMBERS; i++)
  {
    if (IsKeyPressed(numbers[i]) &&
        game->currentTile.isSet)
    {
      int value = numbers[i] - 48;
      push(&game->undoStack, (Action){
                                 .newValue = value,
                                 .oldValue = game->board[game->currentTile.x][game->currentTile.y].value,
                                 .newHidden = false,
                                 .oldHidden = game->board[game->currentTile.x][game->currentTile.y].hidden,
                                 .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
      game->board[game->currentTile.x][game->currentTile.y].value = value;
      game->board[game->currentTile.x][game->currentTile.y].hidden = false;
    }
  }
}

void isActionClicked(Game *game, Vector2 mousePos)
{
  bool withinUndoButton = mousePos.x > game->undoButton.top_left.x && mousePos.x < game->undoButton.bottom_right.x &&
                          mousePos.y > game->undoButton.top_left.y && mousePos.y < game->undoButton.bottom_right.y;
  if (withinUndoButton)
  {
    game->undoButton.selected = true;
  }
  else
  {
    game->undoButton.selected = false;
  }

  if (withinUndoButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    undo(game, &game->undoStack, &game->redoStack);
  }

  bool withinRedoButton = mousePos.x > game->redoButton.top_left.x && mousePos.x < game->redoButton.bottom_right.x &&
                          mousePos.y > game->redoButton.top_left.y && mousePos.y < game->redoButton.bottom_right.y;
  if (withinRedoButton)
  {
    game->redoButton.selected = true;
  }
  else
  {
    game->redoButton.selected = false;
  }

  if (withinRedoButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    redo(game, &game->undoStack, &game->redoStack);
  }

  // Action
  bool withinClearCellButton = mousePos.x > game->clearCellButton.top_left.x && mousePos.x < game->clearCellButton.bottom_right.x &&
                          mousePos.y > game->clearCellButton.top_left.y && mousePos.y < game->clearCellButton.bottom_right.y;
  if (withinClearCellButton)
  {
    game->clearCellButton.selected = true;
  }
  else
  {
    game->clearCellButton.selected = false;
  }

  if (withinClearCellButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    clearCell(game);
  }

  // New Game
  bool withinNewGameButton = mousePos.x > game->newGameButton.top_left.x && mousePos.x < game->newGameButton.bottom_right.x &&
                          mousePos.y > game->newGameButton.top_left.y && mousePos.y < game->newGameButton.bottom_right.y;
  if (withinNewGameButton)
  {
    game->newGameButton.selected = true;
  }
  else
  {
    game->newGameButton.selected = false;
  }

  if (withinNewGameButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    newGame(game);
  }
}

void isNumPadPressed(Game *game, Vector2 mousePos)
{
  for (int i = 0; i < NUM_PAD_TILES; i++)
  {
    for (int j = 0; j < NUM_PAD_TILES; j++)
    {
      bool withinNumPad = mousePos.x > game->numPad[i][j].top_left.x && mousePos.x < game->numPad[i][j].bottom_right.x &&
                          mousePos.y > game->numPad[i][j].top_left.y && mousePos.y < game->numPad[i][j].bottom_right.y;
      if (withinNumPad)
      {
        game->numPad[i][j].selected = true;
      }
      else
      {
        game->numPad[i][j].selected = false;
      }

      if (withinNumPad &&
          game->currentTile.isSet &&
          !game->board[game->currentTile.x][game->currentTile.y].fixed &&
          IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
      {
        printf("numpad clicked at row %d, col %d, value %d\n", game->currentTile.x, game->currentTile.y, game->numPad[i][j].value);
        push(&game->undoStack, (Action){
                                   .newValue = game->numPad[i][j].value,
                                   .oldValue = game->board[game->currentTile.x][game->currentTile.y].value,
                                   .newHidden = false,
                                   .oldHidden = game->board[game->currentTile.x][game->currentTile.y].hidden,
                                   .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
        game->board[game->currentTile.x][game->currentTile.y].value = game->numPad[i][j].value;
        game->board[game->currentTile.x][game->currentTile.y].hidden = false;
      }
    }
  }
}

void isBoardPressed(Game *game, Vector2 mousePos)
{
  int boardEnd = game->layout.boardEnd;

  // Check condition
  if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON) ||
      (game->currentTile.isSet &&
       (mousePos.x > boardEnd || mousePos.x < PADDING ||
        mousePos.y > boardEnd || mousePos.y < PADDING)))
  {
    return;
  }

  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      bool withinTile =
          mousePos.x > game->board[i][j].top_left.x && mousePos.x < game->board[i][j].bottom_right.x &&
          mousePos.y > game->board[i][j].top_left.y && mousePos.y < game->board[i][j].bottom_right.y;
      if (withinTile)
      {
        game->currentTile.x = i;
        game->currentTile.y = j;
        game->currentTile.isSet = true;
      }
    }
  }
}