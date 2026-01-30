#include "input.h"
#include "game.h"
#include "stdio.h"
#include "stack.h"
#include "utils.h"

void handleInput(Game *game)
{
  Vector2 mousePos = GetMousePosition();
  isNumPadPressed(game, mousePos);
  isActionClicked(game, mousePos);
  isBoardPressed(game, mousePos);
  handleKeyboard(game);
}

void handleKeyboard(Game *game)
{
  int numbers[ALLOWED_NUMBERS] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE};

  Tile *selectedTile = &game->board[game->currentTile.x][game->currentTile.y];

  for (int i = 0; i < ALLOWED_NUMBERS; i++)
  {
    if (IsKeyPressed(numbers[i]) &&
        game->currentTile.isSet)
    {
      int value = numbers[i] - 48;
      Vec2i numPadPrevPos = getNumPadPosition(selectedTile->value);
      Vec2i numPadCurrPos = getNumPadPosition(value);

      if (game->numPad[numPadCurrPos.x][numPadCurrPos.y].isCompleted)
      {
        return;
      }

      push(&game->undoStack, (Action){
                                 .newValue = value,
                                 .oldValue = selectedTile->value,
                                 .newHidden = false,
                                 .oldHidden = selectedTile->hidden,
                                 .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
      selectedTile->value = value;
      selectedTile->hidden = false;

      game->numPad[numPadPrevPos.x][numPadPrevPos.y].isCompleted = isDigitCompleted(game, game->numPad[numPadPrevPos.x][numPadPrevPos.y].value);
      game->numPad[numPadCurrPos.x][numPadCurrPos.y].isCompleted = isDigitCompleted(game, game->numPad[numPadCurrPos.x][numPadCurrPos.y].value);
    }
  }
}

void isActionClicked(Game *game, Vector2 mousePos)
{
  bool withinUndoButton = mousePos.x > game->undoButton.top_left.x && mousePos.x < game->undoButton.bottom_right.x &&
                          mousePos.y > game->undoButton.top_left.y && mousePos.y < game->undoButton.bottom_right.y;
  if (withinUndoButton)
  {
    game->undoButton.isHovered = true;
  }
  else
  {
    game->undoButton.isHovered = false;
  }

  if (withinUndoButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    undo(game, &game->undoStack, &game->redoStack);
  }

  bool withinRedoButton = mousePos.x > game->redoButton.top_left.x && mousePos.x < game->redoButton.bottom_right.x &&
                          mousePos.y > game->redoButton.top_left.y && mousePos.y < game->redoButton.bottom_right.y;
  if (withinRedoButton)
  {
    game->redoButton.isHovered = true;
  }
  else
  {
    game->redoButton.isHovered = false;
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
    game->clearCellButton.isHovered = true;
  }
  else
  {
    game->clearCellButton.isHovered = false;
  }

  if (withinClearCellButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    clearCell(game);
    isSelectedValueCompleted(game);
  }

  // New Game
  bool withinNewGameButton = mousePos.x > game->newGameButton.top_left.x && mousePos.x < game->newGameButton.bottom_right.x &&
                             mousePos.y > game->newGameButton.top_left.y && mousePos.y < game->newGameButton.bottom_right.y;
  if (withinNewGameButton)
  {
    game->newGameButton.isHovered = true;
  }
  else
  {
    game->newGameButton.isHovered = false;
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
      Tile *selectedTile = &game->board[game->currentTile.x][game->currentTile.y];
      bool withinNumPad = mousePos.x > game->numPad[i][j].top_left.x && mousePos.x < game->numPad[i][j].bottom_right.x &&
                          mousePos.y > game->numPad[i][j].top_left.y && mousePos.y < game->numPad[i][j].bottom_right.y;
      if (withinNumPad)
      {
        game->numPad[i][j].isHovered = true;
      }
      else
      {
        game->numPad[i][j].isHovered = false;
      }

      if (withinNumPad &&
          game->currentTile.isSet &&
          !selectedTile->fixed &&
          IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
          !game->numPad[i][j].isCompleted)
      {
        // printf("numpad clicked at row %d, col %d, value %d\n", game->currentTile.x, game->currentTile.y, game->numPad[i][j].value);
        push(&game->undoStack, (Action){
                                   .newValue = game->numPad[i][j].value,
                                   .oldValue = selectedTile->value,
                                   .newHidden = false,
                                   .oldHidden = selectedTile->hidden,
                                   .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
        int previousValue = selectedTile->value;
        selectedTile->value = game->numPad[i][j].value;
        selectedTile->hidden = false;

        isPreviousValueCompleted(game, previousValue);
        game->numPad[i][j].isCompleted = isDigitCompleted(game, game->numPad[i][j].value);
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