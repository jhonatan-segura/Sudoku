#include "input.h"
#include "game.h"
#include "stdio.h"
#include "stack.h"
#include "utils.h"

void handleInput(Game *game)
{
  Vector2 mousePos = GetMousePosition();

  if (game->difficultyModeButton.isEnabled) {
    handleDifficultyChange(game, mousePos);
    return;
  }

  if (!game->isGameOver)
  {
    if (!game->isGamePaused)
    {
      isNumPadPressed(game, mousePos);
      isBoardPressed(game, mousePos);
      handleKeyboardDigits(game);
    }
    handlePause(game);
  }

  handleMouse(game, mousePos);
}

void handlePause(Game *game)
{
  if (IsKeyPressed(KEY_SPACE))
  {
    game->isGamePaused = !game->isGamePaused;
  }
}

void handleUnpauseGame(Game *game)
{
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    game->isGamePaused = !game->isGamePaused;
  }
}

void handleKeyboardDigits(Game *game)
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

      int previousValue = selectedTile->value;
      bool previousIsHidden = selectedTile->hidden;
      bool previousIsValid = selectedTile->isValid;

      selectedTile->value = value;
      game->currentTile.tempValue = value;
      selectedTile->hidden = false;

      isNumPadValueCompleted(game, game->numPad[numPadPrevPos.x][numPadPrevPos.y].value);
      isNumPadValueCompleted(game, game->numPad[numPadCurrPos.x][numPadCurrPos.y].value);

      // Check errors
      bool isValid = checkErrors(game);
      if (isValid && (previousIsHidden || game->visibleTilesCount == TOTAL_TILES - 1))
      {
        game->visibleTilesCount++;
      }
      if (game->visibleTilesCount == TOTAL_TILES)
      {
        checkGameCompleted(game);
      }

      push(&game->undoStack,
           (Action){
               .newValue = value,
               .oldValue = previousValue,
               .newHidden = false,
               .oldHidden = previousIsHidden,
               .oldIsValid = previousIsValid,
               .newIsValid = selectedTile->isValid,
               .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
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

bool isTileHovered(Game *game, int x, int y, Vector2 mousePos)
{
  bool withinUndoButton = mousePos.x > game->board[x][y].top_left.x && mousePos.x < game->board[x][y].bottom_right.x &&
                          mousePos.y > game->board[x][y].top_left.y && mousePos.y < game->board[x][y].bottom_right.y;

  if (withinUndoButton)
  {
    game->hoveredTile.x = x;
    game->hoveredTile.y = y;
    game->hoveredTile.isSet = true;
  }
  else
  {
    game->hoveredTile.x = -1;
    game->hoveredTile.y = -1;
    game->hoveredTile.isSet = false;
  }
  return withinUndoButton;
}

bool isButtonClicked(Button *button, Vector2 mousePos)
{
  return isButtonHovered(button, mousePos) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

void handleMouse(Game *game, Vector2 mousePos)
{
  if (isButtonClicked(&game->gameOverButton, mousePos))
  {
    newGame(game, game->difficultyIndex);
  }

  if (game->isGameOver || game->isGamePaused)
  {
    if (game->isGamePaused)
    {
      handleUnpauseGame(game);
    }
    return;
  }

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
    clearNumPadValueCompleted(game);
  }

  if (isButtonClicked(&game->newGameButton, mousePos))
  {
    newGame(game, game->difficultyIndex);
  }

  if (isButtonClicked(&game->pauseButton, mousePos))
  {
    game->isGamePaused = !game->isGamePaused;
  }

  if (isButtonClicked(&game->difficultyModeButton, mousePos))
  {
    game->difficultyModeButton.isEnabled = true;
  }
}

void handleDifficultyChange(Game *game, Vector2 mousePos)
{
  for (int i = 0; i < DIFFICULTIES; i++)
  {
    if (isButtonClicked(&game->difficultyButtons[i], mousePos))
    {
      newGame(game, i);
    }
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
        int previousValue = selectedTile->value;
        bool previousIsHidden = selectedTile->hidden;
        bool previousIsValid = selectedTile->isValid;

        selectedTile->value = game->numPad[i][j].value;
        game->currentTile.tempValue = game->numPad[i][j].value;
        selectedTile->hidden = false;

        setPreviousValueNotCompleted(game, previousValue);
        isNumPadValueCompleted(game, game->numPad[i][j].value);

        // Check errors
        bool isValid = checkErrors(game);
        if (isValid && (previousIsHidden || game->visibleTilesCount == TOTAL_TILES - 1))
        {
          game->visibleTilesCount++;
        }
        if (game->visibleTilesCount == TOTAL_TILES)
        {
          checkGameCompleted(game);
        }

        push(&game->undoStack,
             (Action){
                 .newValue = game->numPad[i][j].value,
                 .oldValue = previousValue,
                 .newHidden = false,
                 .oldHidden = previousIsHidden,
                 .oldIsValid = previousIsValid,
                 .newIsValid = selectedTile->isValid,
                 .position = (Vec2){(float)game->currentTile.x, (float)game->currentTile.y}});
      }
    }
  }
}

void isBoardPressed(Game *game, Vector2 mousePos)
{
  int boardEnd = game->layout.boardEnd;

  // Early return if left click isn't pressed or there isn't a tile selected
  // Or mouse is hovering out of the board.
  if ((game->currentTile.isSet &&
       (mousePos.x > boardEnd || mousePos.x < PADDING ||
        mousePos.y > boardEnd || mousePos.y < PADDING)))
  {
    game->hoveredTile.isSet = false;
    return;
  }

  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
      {
        bool isHovered = isTileHovered(game, i, j, mousePos);
        if (isHovered)
          return;
      }

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