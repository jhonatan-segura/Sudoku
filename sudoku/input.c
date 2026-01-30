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

bool isNumPadButtonHovered(NumPadButton *button, Vector2 mousePos)
{
  bool withinUndoButton = mousePos.x > button->top_left.x && mousePos.x < button->bottom_right.x &&
                          mousePos.y > button->top_left.y && mousePos.y < button->bottom_right.y;
  if (withinUndoButton)
  {
    button->isHovered = true;
  }
  else
  {
    button->isHovered = false;
  }
  return withinUndoButton;
}

bool isButtonHovered(Button *button, Vector2 mousePos)
{
  bool withinUndoButton = mousePos.x > button->top_left.x && mousePos.x < button->bottom_right.x &&
                          mousePos.y > button->top_left.y && mousePos.y < button->bottom_right.y;
  if (withinUndoButton)
  {
    button->isHovered = true;
  }
  else
  {
    button->isHovered = false;
  }
  return withinUndoButton;
}

bool isButtonClicked(Button *button, Vector2 mousePos)
{
  return isButtonHovered(button, mousePos) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

void isActionClicked(Game *game, Vector2 mousePos)
{
  if (isButtonClicked(&game->undoButton, mousePos))
  {
    undo(game, &game->undoStack, &game->redoStack);
  }

  if (isButtonClicked(&game->redoButton, mousePos))
  {
    redo(game, &game->undoStack, &game->redoStack);
  }

  if (isButtonClicked(&game->clearCellButton, mousePos))
  {
    clearCell(game);
    setSelectedValueCompleted(game);
  }

  if (isButtonClicked(&game->newGameButton, mousePos))
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
      bool withinNumPad = isNumPadButtonHovered(&game->numPad[i][j], mousePos);

      if (withinNumPad &&
          game->currentTile.isSet &&
          !selectedTile->fixed &&
          IsMouseButtonReleased(MOUSE_LEFT_BUTTON) &&
          !game->numPad[i][j].isCompleted)
      {
        push(&game->undoStack, (Action){
                                   .newValue = game->numPad[i][j].value,
                                   .oldValue = selectedTile->value,
                                   .newHidden = false,
                                   .oldHidden = selectedTile->hidden,
                                   .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
        int previousValue = selectedTile->value;
        selectedTile->value = game->numPad[i][j].value;
        selectedTile->hidden = false;

        setPreviousValueNotCompleted(game, previousValue);
        game->numPad[i][j].isCompleted = isDigitCompleted(game, game->numPad[i][j].value);
      }
    }
  }
}

void isBoardPressed(Game *game, Vector2 mousePos)
{
  int boardEnd = game->layout.boardEnd;

  // Early return if left click isn't pressed or there isn't a tile selected
  // Or mouse is hovering out of the board.
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