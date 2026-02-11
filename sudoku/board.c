#include "board.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <stdbool.h>

int randValue(int min, int max)
{
  int range = max - min + 1;

  return (rand() % range) + min;
}

bool isValidRow(Tile board[][TILES], Position *currentPos)
{
  for (int j = 0; j < TILES; j++)
    // !board[currentPos->x][j].hidden: Triggers the error on the tile if the value is already present as a visible value.
    // Without this validation the error on the tile is triggered if the value isn't the target value.

    if (board[currentPos->x][j].value == currentPos->tempValue && j != currentPos->y && !board[currentPos->x][j].hidden)
      return false;

  return true;
}

bool isValidCol(Tile board[][TILES], Position *currentPos)
{
  for (int i = 0; i < TILES; i++)
    if (board[i][currentPos->y].value == currentPos->tempValue && i != currentPos->x && !board[i][currentPos->y].hidden)
      return false;

  return true;
}

bool isValidSector(Tile board[][TILES], Position *currentPos)
{
  int lowV = (currentPos->x / 3) * 3;
  int highV = lowV + 2;
  int lowH = (currentPos->y / 3) * 3;
  int highH = lowH + 2;
  for (int x = lowV; x <= highV; x++)
    for (int y = lowH; y <= highH; y++)
      if (board[x][y].value == currentPos->tempValue &&
          x != currentPos->x &&
          y != currentPos->y &&
          !board[x][y].hidden)
        return false;

  return true;
}

bool isAllowedCell(Tile board[][TILES], Position *currentPos)
{
  return isValidRow(board, currentPos) && isValidCol(board, currentPos) && isValidSector(board, currentPos);
}

bool isAllowedCellCreatingBoard(Tile board[][TILES], Position *currentPos)
{
  return isAllowedCell(board, currentPos) && notAttemptedYet(board, currentPos);
}

void goBack(Position *currentPos)
{
  if (currentPos->y == 0)
  {
    currentPos->x--;
    currentPos->y = TILES - 1;
  }
  else
  {
    currentPos->y--;
  }
}

bool goForth(Position *currentPos)
{
  if (currentPos->y == (TILES - 1) && currentPos->x == (TILES - 1))
  {
    return true;
  }
  if (currentPos->y != (TILES - 1))
  {
    currentPos->y++;
  }
  else
  {
    currentPos->y = 0;
    currentPos->x++;
  }
  return false;
}

void setAttempt(Tile board[][TILES], Position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    if (board[currentPos->x][currentPos->y].attempts[x] == currentPos->tempValue)
    {
      break;
    }

    if (board[currentPos->x][currentPos->y].attempts[x] == 0)
    {
      board[currentPos->x][currentPos->y].attempts[x] = currentPos->tempValue;
      break;
    }
  }
}

void setAttemptCurrentPos(Position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    if (currentPos->attempts[x] == currentPos->tempValue)
    {
      break;
    }

    if (currentPos->attempts[x] == 0)
    {
      currentPos->attempts[x] = currentPos->tempValue;
      break;
    }
  }
}

bool notAttemptedYet(Tile board[][TILES], Position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    if (board[currentPos->x][currentPos->y].attempts[x] == currentPos->tempValue)
    {
      return false;
    }
  }
  return true;
}

void printAttemptsCurrentPos(Position *currentPos)
{
  printf("CurrentAttempts:\n");
  for (int x = 0; x < TILES; x++)
  {
    printf("v: %d  ", currentPos->attempts[x]);
  }
  printf("\n");
}

bool areAllOptionsAttemptedCurrentPos(Position *currentPos)
{
  if (currentPos->attempts[TILES - 1] != 0)
  {
    return true;
  }
  return false;
}

void resetAttempts(Tile board[][TILES], Position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    board[currentPos->x][currentPos->y].attempts[x] = 0;
  }
}

void resetAttemptsCurrentPos(Position *currentPos)
{
  memset(currentPos->attempts, 0, sizeof(currentPos->attempts));
}

bool isRandomValueValid(Tile board[][TILES], Position *currentPos)
{
  do
  {
    if (areAllOptionsAttemptedCurrentPos(currentPos))
    {
      return false;
    }
    currentPos->tempValue = randValue(1, 9);
    setAttemptCurrentPos(currentPos);
  } while (!isAllowedCellCreatingBoard(board, currentPos));
  return true;
}

void printBoard(Tile board[][TILES])
{
  for (int x = 0; x < TILES; x++)
  {
    if (x % 3 == 0 && x != 0)
    {
      printf("- - - - - - - - - - -\n");
    }
    for (int y = 0; y < TILES; y++)
    {
      if (y % 3 == 0 && y != 0)
      {
        printf("| ");
      }
      printf("%d ", board[x][y].value);
    }
    printf("\n");
  }
  printf("\n");
}

bool fillCell(Tile board[][TILES], Position *currentPos)
{
  bool isValid = isRandomValueValid(board, currentPos);

  if (isValid)
  {
    setAttempt(board, currentPos);
    board[currentPos->x][currentPos->y].value = currentPos->tempValue;
    board[currentPos->x][currentPos->y].targetValue = currentPos->tempValue;
    return true;
  }
  return false;
}

void solver(Tile board[][TILES])
{
  bool completed = false;
  Position currentPos;
  currentPos.x = 0;
  currentPos.y = 0;
  currentPos.tempValue = 0;
  while (!completed)
  {
    resetAttemptsCurrentPos(&currentPos);
    bool filledCell = fillCell(board, &currentPos);
    if (filledCell)
    {
      completed = goForth(&currentPos);
    }
    else
    {
      resetAttempts(board, &currentPos);
      board[currentPos.x][currentPos.y].value = 0;
      goBack(&currentPos);
    }
  }
}

void initBoard(Tile board[][TILES])
{
  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      board[i][j].value = 0;
      board[i][j].hidden = false;
      board[i][j].fixed = true;
      board[i][j].isValid = true;
    }
  }
}

void hideTiles(Tile board[][TILES], int numTiles)
{
  int x, y;
  do
  {
    x = randValue(0, 8);
    y = randValue(0, 8);
    if (board[x][y].hidden == false)
    {
      --numTiles;
    }
    board[x][y].hidden = true;
    board[x][y].fixed = false;
  } while (numTiles > 0);
}
