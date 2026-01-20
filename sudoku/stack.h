#pragma once

#include "game_types.h" // Action, Stack

void push(Stack **stack, Action action);
Action pop(Stack **stack);
void freeStack(Stack *stack);
