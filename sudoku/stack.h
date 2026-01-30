#pragma once

#include "game_types.h"

void push(Stack **stack, Action action);
Action pop(Stack **stack);
void freeStack(Stack *stack);
void printStack(Stack *stack);