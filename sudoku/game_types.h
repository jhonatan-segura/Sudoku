#pragma once

#include "utils.h"
#include <stdbool.h>

typedef struct
{
  int newValue;
  int oldValue;
  bool newHidden;
  bool oldHidden;
  Vec2 position;
} Action;

typedef struct Stack
{
  Action action;
  struct Stack *next;
} Stack;
