#pragma once

#include <stdbool.h>

typedef struct
{
  float x;
  float y;
} Vec2;

typedef struct
{
  int x;
  int y;
} Vec2i;

typedef enum
{
  EASY = 0,
  MEDIUM = 1,
  HARD = 2
} DifficultyIndex;

typedef struct
{
  int tilesToHide;
  char *text;
} Difficulty;

typedef struct
{
  int newValue;
  int oldValue;
  bool newHidden;
  bool oldHidden;
  bool newIsValid;
  bool oldIsValid;
  Vec2 position;
} Action;

typedef struct Stack
{
  Action action;
  struct Stack *next;
} Stack;