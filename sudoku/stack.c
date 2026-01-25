#include "stack.h"
#include <stdlib.h>
#include <stdio.h>

void push(Stack **stack, Action action)
{
  Stack *new_node = (Stack *)malloc(sizeof(Stack));
  if (new_node == NULL)
  {
    printf("Error: no se pudo asignar memoria\n");
    exit(1);
  }

  // Avoid pushing the same value at the same Position multiple times in a row.
  if (*stack != NULL &&
      (*stack)->action.newValue == action.newValue &&
      (*stack)->action.position.x == action.position.x &&
      (*stack)->action.position.y == action.position.y)
  {
    return;
  }

  new_node->action = action;
  new_node->next = *stack;
  *stack = new_node;
}

Action pop(Stack **undo_stack)
{
  Stack *temp = *undo_stack;
  Action action = temp->action;
  *undo_stack = (*undo_stack)->next;
  free(temp);
  return action;
}

void freeStack(Stack *stack)
{
  Stack *temp;
  while (stack != NULL)
  {
    temp = stack;
    stack = stack->next;
    free(temp);
  }
}

void printStack(Stack *stack)
{
  printf("Elementos del stack:\n");
  int count = 0;
  while (stack != NULL)
  {
    printf("%d:\n", count++);
    printf(" - Anterior %d\n", stack->action.oldValue);
    printf(" - Nuevo %d\n", stack->action.newValue);
    stack = stack->next;
  }
}
