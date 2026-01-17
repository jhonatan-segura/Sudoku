#include "raylib.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

#define TILES 9
#define NUM_PAD_TILES 3
#define WIDTH 1200
#define HEIGHT 700
#define PADDING 40
#define HUD_GAP 20
#define TEXT_PADDING 18
#define LINE_THICKNESS 3.0

#define ALLOWED_NUMBERS 9

// difficulty
#define EASY 40
#define MEDIUM 50
#define HARD 60

typedef struct
{
  int value;
  int targetValue;
  int attempts[TILES];
  bool hidden;
  bool fixed;
  Vector2 top_left;
  Vector2 bottom_right;
} tile;

typedef struct
{
  int newValue;
  int oldValue;
  bool newHidden;
  bool oldHidden;
  Vector2 position;
} Action;

typedef struct
{
  int value;
  bool selected;
  Color color;
  Vector2 top_left;
  Vector2 bottom_right;
} Button;

typedef struct
{
  int x;
  int y;
  int tempValue;
  int attempts[TILES];
} position;

typedef struct str_s
{
  Action action;
  struct str_s *next;
} Stack;

typedef enum
{
  UNDO,
  REDO
} PossibleMoves;

position currentTile;

tile board[TILES][TILES];
Button numPad[NUM_PAD_TILES][NUM_PAD_TILES];
Button undoButton;
Button redoButton;
float BOARD_SIZE;
int BOARD_END_FULL;
Vector2 HUD_SIZE;
Vector2 NUMPAD_BUTTON_SIZE;

Stack *undo_stack = NULL;
Stack *redo_stack = NULL;

int BOARD_END;
float TILE_SIZE;
float HALF_TILE_SIZE;
float HALF_NUM_TILE_SIZE;

float prevTime = 0.0f;
float now = 0.0f;
int minutes = 0;
int seconds = 0;

// static void UpdateGame(void); // Update game (one frame)
// static void DrawGame(void);   // Draw game (one frame)
// static void UnloadGame(void); // Unload game
void initGame();
void initTime();
bool isValidCol(position *currentPos);
bool isValidRow(position *currentPos);
bool notAttemptedYet(position *currentPos);
bool fillCell(position *currentPos);

void drawBoard(void);
void drawHUD(void);
void drawClock(void);
void isBoardPressed();
void handleMouse();
void handleKeyboard();
int randValue(int min, int max);
void resetAttempts(position *currentPos);
bool isRandomValueValid(position *currentPos);
void printBoard();
void initBoard();
void goBack(position *currentPos);
bool goForth(position *currentPos);

void push(Stack **stack, Action action);
Action pop(Stack **undo_stack);
void moveStacks(Stack **stack1, Stack **stack2, PossibleMoves move);
void undo(Stack **undo_stack, Stack **redo_stack);
void redo(Stack **undo_stack, Stack **redo_stack);
void printValues(Stack *stack);
void freeStack(Stack *stack);

int main(void)
{
  const int screenWidth = WIDTH;
  const int screenHeight = HEIGHT;
  BOARD_SIZE = 0.6f;

  BOARD_END_FULL = WIDTH * BOARD_SIZE;
  HUD_SIZE = (Vector2){
      BOARD_END_FULL + PADDING,
      WIDTH - PADDING};

  int button_size = (HUD_SIZE.y - HUD_SIZE.x) / 3 - HUD_GAP;
  NUMPAD_BUTTON_SIZE = (Vector2){button_size, button_size};

  BOARD_END = BOARD_END_FULL - PADDING;

  TILE_SIZE = (BOARD_END - PADDING) / (float)TILES;
  HALF_TILE_SIZE = TILE_SIZE / 2.0f;
  HALF_NUM_TILE_SIZE = NUMPAD_BUTTON_SIZE.x / 2.0f;

  currentTile.x = -1;
  currentTile.y = -1;

  InitWindow(screenWidth, screenHeight, "Sudoku");

  SetTargetFPS(60);

  initGame();
  // Main game loop
  while (!WindowShouldClose())
  {
    // Update

    // Draw
    BeginDrawing();

    ClearBackground(RAYWHITE);
    // Input
    handleMouse();
    handleKeyboard();

    drawBoard();
    drawHUD();
    drawClock();

    EndDrawing();
  }

  // De-Initialization
  freeStack(undo_stack);
  freeStack(redo_stack);
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context

  return 0;
}

void drawBoard()
{
  // printf("boardsize %d\n", BOARD_SIZE);
  // printf("boardsize/9 %d\n", BOARD_SIZE / 9);
  // printf("tile %d\n", TILE_SIZE);
  for (int i = 0; i < TILES + 1; i++)
  {
    int coord = i * TILE_SIZE + PADDING;
    Vector2 start_hor = (Vector2){PADDING, coord};
    Vector2 end_hor = (Vector2){BOARD_END, coord};
    DrawLineV(start_hor, end_hor, BLACK);

    Vector2 start_ver = (Vector2){coord, PADDING};
    Vector2 end_ver = (Vector2){coord, BOARD_END};
    DrawLineV(start_ver, end_ver, BLACK);

    if (i % 3 == 0)
    {
      DrawLineEx(start_hor, end_hor, LINE_THICKNESS, BLACK);
      DrawLineEx(start_ver, end_ver, LINE_THICKNESS, BLACK);
    }
  }

  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      int x1 = j * TILE_SIZE + PADDING;
      int y1 = i * TILE_SIZE + PADDING;
      int x2 = (j + 1) * TILE_SIZE + PADDING;
      int y2 = (i + 1) * TILE_SIZE + PADDING;
      board[i][j].top_left = (Vector2){x1, y1};
      board[i][j].bottom_right = (Vector2){x2, y2};

      if (currentTile.x == i && currentTile.y == j)
      {
        Rectangle rect = {
            .x = x1,
            .y = y1,
            .width = TILE_SIZE,
            .height = TILE_SIZE};
        DrawRectangleLinesEx(rect, 3.0, BLUE);
      }

      if (board[i][j].hidden)
      {
        continue;
      }

      int x_coord = x1 + HALF_TILE_SIZE - 5;
      int y_coord = y1 + TEXT_PADDING;
      const char *cellText = TextFormat("%i", board[i][j].value);
      Color textColor = board[i][j].fixed ? BLACK : GRAY;
      DrawText(cellText, x_coord, y_coord, 28, textColor);
    }
  }
}

void drawHUD()
{
  // Draw numpad
  int numpad_count = 1;
  for (int i = 0; i < NUM_PAD_TILES; i++)
  {
    for (int j = 0; j < NUM_PAD_TILES; j++)
    {
      // Coordinates to draw numpad buttons and store their positions to detect clicks.
      int x1 = j * (NUMPAD_BUTTON_SIZE.x + HUD_GAP) + HUD_SIZE.x;
      int y1 = i * (NUMPAD_BUTTON_SIZE.x + HUD_GAP) + 280;
      int x2 = x1 + NUMPAD_BUTTON_SIZE.x;
      int y2 = y1 + NUMPAD_BUTTON_SIZE.x;

      numPad[i][j].top_left = (Vector2){x1, y1};
      numPad[i][j].bottom_right = (Vector2){x2, y2};
      numPad[i][j].value = numpad_count;
      numPad[i][j].color = numPad[i][j].selected ? LIGHTGRAY : WHITE;

      Rectangle rect = {
          .x = x1,
          .y = y1,
          .width = NUMPAD_BUTTON_SIZE.x,
          .height = NUMPAD_BUTTON_SIZE.y};
      Vector2 rectPos = (Vector2){rect.x, rect.y};
      Vector2 rectSize = (Vector2){rect.width, rect.height};
      DrawRectangleV(rectPos, rectSize, numPad[i][j].color);
      DrawRectangleLinesEx(rect, 2.0, BLACK);

      const char *cellText = TextFormat("%i", numpad_count++);
      int num_x = x1 + HALF_NUM_TILE_SIZE - 5;
      int num_y = y1 + HALF_NUM_TILE_SIZE - 14;
      DrawText(cellText, num_x, num_y, 28, BLACK);
    }
  }

  // Draw undo/redo buttons
  int undo_x = HUD_SIZE.x;
  int undo_y = PADDING;

  Rectangle undoRect = {
      .x = undo_x,
      .y = undo_y,
      .width = NUMPAD_BUTTON_SIZE.x,
      .height = NUMPAD_BUTTON_SIZE.y};

  undoButton.value = 0;
  undoButton.top_left = (Vector2){undo_x, undo_y};
  undoButton.bottom_right = (Vector2){undo_x + NUMPAD_BUTTON_SIZE.x, undo_y + NUMPAD_BUTTON_SIZE.y};
  Vector2 undoPos = (Vector2){undoRect.x, undoRect.y};
  Vector2 undoSize = (Vector2){undoRect.width, undoRect.height};
  undoButton.color = undoButton.selected ? LIGHTGRAY : WHITE;
  DrawRectangleV(undoPos, undoSize, undoButton.color);

  DrawRectangleLinesEx(undoRect, 2.0, BLACK);
  DrawText("<", undo_x + HALF_NUM_TILE_SIZE - 5, undo_y + HALF_NUM_TILE_SIZE - 14, 35, BLACK);

  // redo
  int redo_x = HUD_SIZE.x + NUMPAD_BUTTON_SIZE.x + HUD_GAP;
  int redo_y = PADDING;

  Rectangle redoRect = {
      .x = redo_x,
      .y = redo_y,
      .width = NUMPAD_BUTTON_SIZE.x,
      .height = NUMPAD_BUTTON_SIZE.y};

  redoButton.value = 0;
  redoButton.top_left = (Vector2){redo_x, redo_y};
  redoButton.bottom_right = (Vector2){redo_x + NUMPAD_BUTTON_SIZE.x, redo_y + NUMPAD_BUTTON_SIZE.y};
  Vector2 redoPos = (Vector2){redoRect.x, redoRect.y};
  Vector2 redoSize = (Vector2){redoRect.width, redoRect.height};
  redoButton.color = redoButton.selected ? LIGHTGRAY : WHITE;
  DrawRectangleV(redoPos, redoSize, redoButton.color);

  DrawRectangleLinesEx(redoRect, 2.0, BLACK);
  DrawText(">", redo_x + HALF_NUM_TILE_SIZE - 5, redo_y + HALF_NUM_TILE_SIZE - 14, 35, BLACK);
}

void drawClock()
{
  now = GetTime();
  if (now - prevTime >= 1.0f)
  {
    prevTime = now;
    seconds++;
  }
  if (seconds == 60)
  {
    minutes++;
    seconds = 0.0;
  }

  const char *time = TextFormat("%02d:%02d", minutes, seconds);
  // printf("Time: %02d\n", minutes);
  DrawText(time, PADDING, 10, 28, BLACK);
}

void isUndoRedoPressed(Vector2 mousePos)
{
  bool withinUndoButton = mousePos.x > undoButton.top_left.x && mousePos.x < undoButton.bottom_right.x &&
                          mousePos.y > undoButton.top_left.y && mousePos.y < undoButton.bottom_right.y;
  if (withinUndoButton)
  {
    undoButton.selected = true;
  }
  else
  {
    undoButton.selected = false;
  }

  if (withinUndoButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    undo(&undo_stack, &redo_stack);
  }

  bool withinRedoButton = mousePos.x > redoButton.top_left.x && mousePos.x < redoButton.bottom_right.x &&
                          mousePos.y > redoButton.top_left.y && mousePos.y < redoButton.bottom_right.y;
  if (withinRedoButton)
  {
    redoButton.selected = true;
  }
  else
  {
    redoButton.selected = false;
  }

  if (withinRedoButton && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    redo(&undo_stack, &redo_stack);
  }
}

void isNumPadPressed(Vector2 mousePos)
{
  for (int i = 0; i < NUM_PAD_TILES; i++)
  {
    for (int j = 0; j < NUM_PAD_TILES; j++)
    {
      bool withinNumPad = mousePos.x > numPad[i][j].top_left.x && mousePos.x < numPad[i][j].bottom_right.x &&
                          mousePos.y > numPad[i][j].top_left.y && mousePos.y < numPad[i][j].bottom_right.y;
      if (withinNumPad)
      {
        numPad[i][j].selected = true;
      }
      else
      {
        numPad[i][j].selected = false;
      }

      if (withinNumPad &&
          currentTile.x != -1 &&
          currentTile.y != -1 &&
          IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
      {
        printf("numpad clicked at row %d, col %d, value %d\n", currentTile.x, currentTile.y, numPad[i][j].value);
        push(&undo_stack, (Action){
                              .newValue = numPad[i][j].value,
                              .oldValue = board[currentTile.x][currentTile.y].value,
                              .newHidden = false,
                              .oldHidden = board[currentTile.x][currentTile.y].hidden,
                              .position = (Vector2){currentTile.x, currentTile.y}});
        board[currentTile.x][currentTile.y].value = numPad[i][j].value;
        board[currentTile.x][currentTile.y].hidden = false;
      }
    }
  }
}

void handleKeyboard()
{
  int numbers[ALLOWED_NUMBERS] = {KEY_ONE, KEY_TWO, KEY_THREE, KEY_FOUR, KEY_FIVE, KEY_SIX, KEY_SEVEN, KEY_EIGHT, KEY_NINE};
  for (int i = 0; i < ALLOWED_NUMBERS; i++)
  {
    if (IsKeyPressed(numbers[i]) &&
        currentTile.x != -1 &&
        currentTile.y != -1)
    {
      int value = numbers[i] - 48;
      push(&undo_stack, (Action){
                            .newValue = value,
                            .oldValue = board[currentTile.x][currentTile.y].value,
                            .newHidden = false,
                            .oldHidden = board[currentTile.x][currentTile.y].hidden,
                            .position = (Vector2){currentTile.x, currentTile.y}});
      board[currentTile.x][currentTile.y].value = value;
      board[currentTile.x][currentTile.y].hidden = false;
    }
  }
}

void handleMouse()
{
  Vector2 mousePos = GetMousePosition();
  // printf("Mouse over %f, %f\n", mousePos.x, mousePos.y);
  isNumPadPressed(mousePos);
  isUndoRedoPressed(mousePos);
  isBoardPressed(mousePos);
}

void isBoardPressed(Vector2 mousePos)
{
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
  {
    if (currentTile.x != -1 && currentTile.y != -1 &&
        (mousePos.x > BOARD_END || mousePos.x < PADDING ||
         mousePos.y > BOARD_END || mousePos.y < PADDING))
    {
      return;
    }
    for (int i = 0; i < TILES; i++)
    {
      for (int j = 0; j < TILES; j++)
      {
        bool withinTile =
            mousePos.x > board[i][j].top_left.x && mousePos.x < board[i][j].bottom_right.x &&
            mousePos.y > board[i][j].top_left.y && mousePos.y < board[i][j].bottom_right.y;
        if (board[i][j].fixed != true &&
            withinTile)
        {
          currentTile.x = i;
          currentTile.y = j;
        }
        else if (board[i][j].fixed == true &&
                 withinTile)
        {
          currentTile.x = -1;
          currentTile.y = -1;
        }
      }
    }
  }
}

void initTime()
{
  srand(time(NULL));
}

int randValue(int min, int max)
{
  int range = max - min + 1;

  return (rand() % range) + min;
}

bool isValidRow(position *currentPos)
{
  for (int j = 0; j < TILES; j++)
  {
    if (board[currentPos->y][j].value == currentPos->tempValue)
    {
      return false;
    }
  }
  return true;
}

bool isValidCol(position *currentPos)
{
  for (int i = 0; i < TILES; i++)
  {
    if (board[i][currentPos->x].value == currentPos->tempValue)
    {
      return false;
    }
  }
  return true;
}

bool isValidSector(position *currentPos)
{
  int lowV = (currentPos->y / 3) * 3;
  int highV = lowV + 2;
  int lowH = (currentPos->x / 3) * 3;
  int highH = lowH + 2;
  for (int y = lowV; y <= highV; y++)
  {
    for (int x = lowH; x <= highH; x++)
    {
      if (board[y][x].value == currentPos->tempValue)
      {
        return false;
      }
    }
  }
  return true;
}

bool isAllowedCell(position *currentPos)
{
  // printf("value %d  ", currentPos.tempValue);
  // printf("row %d  ", isValidRow());
  // printf("col %d  ", isValidCol());
  // printf("sector %d ", isValidSector());
  // printf("notAttemptedYet %d\n", notAttemptedYet());
  return isValidRow(currentPos) && isValidCol(currentPos) && isValidSector(currentPos) && notAttemptedYet(currentPos);
}

void goBack(position *currentPos)
{
  if (currentPos->x == 0)
  {
    currentPos->y--;
    currentPos->x = TILES - 1;
  }
  else
  {
    currentPos->x--;
  }
}

bool goForth(position *currentPos)
{
  if (currentPos->x == (TILES - 1) && currentPos->y == (TILES - 1))
  {
    return true;
  }
  if (currentPos->x != (TILES - 1))
  {
    currentPos->x++;
  }
  else
  {
    currentPos->x = 0;
    currentPos->y++;
  }
  return false;
}

void setAttempt(position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    if (board[currentPos->y][currentPos->x].attempts[x] == currentPos->tempValue)
    {
      break;
    }

    if (board[currentPos->y][currentPos->x].attempts[x] == 0)
    {
      board[currentPos->y][currentPos->x].attempts[x] = currentPos->tempValue;
      break;
    }
  }
}

void setAttemptCurrentPos(position *currentPos)
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

bool notAttemptedYet(position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    if (board[currentPos->y][currentPos->x].attempts[x] == currentPos->tempValue)
    {
      return false;
    }
  }
  return true;
}

void printAttemptsCurrentPos(position *currentPos)
{
  printf("CurrentAttempts:\n");
  for (int x = 0; x < TILES; x++)
  {
    printf("v: %d  ", currentPos->attempts[x]);
  }
  printf("\n");
}

bool areAllOptionsAttemptedCurrentPos(position *currentPos)
{
  // todas las posibilidades han sido intentadas
  if (currentPos->attempts[TILES - 1] != 0)
  {
    return true;
  }
  return false;
}

void resetAttempts(position *currentPos)
{
  for (int x = 0; x < TILES; x++)
  {
    board[currentPos->y][currentPos->x].attempts[x] = 0;
  }
}

void resetAttemptsCurrentPos(position *currentPos)
{
  memset(currentPos->attempts, 0, sizeof(currentPos->attempts));
}

bool isRandomValueValid(position *currentPos)
{
  do
  {
    if (areAllOptionsAttemptedCurrentPos(currentPos))
    {
      return false;
    }
    currentPos->tempValue = randValue(1, 9);
    setAttemptCurrentPos(currentPos);
  } while (!isAllowedCell(currentPos));
  return true;
}

void printBoard()
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

bool fillCell(position *currentPos)
{
  bool isValid = isRandomValueValid(currentPos);

  if (isValid)
  {
    setAttempt(currentPos);
    board[currentPos->y][currentPos->x].value = currentPos->tempValue;
    board[currentPos->y][currentPos->x].targetValue = currentPos->tempValue;
    return true;
  }
  return false;
}

void solver()
{
  bool completed = false;
  position currentPos;
  currentPos.x = 0;
  currentPos.y = 0;
  currentPos.tempValue = 0;
  while (!completed)
  {
    resetAttemptsCurrentPos(&currentPos);
    bool filledCell = fillCell(&currentPos);
    if (filledCell)
    {
      completed = goForth(&currentPos);
    }
    else
    {
      // reset
      resetAttempts(&currentPos);
      board[currentPos.y][currentPos.x].value = 0;
      goBack(&currentPos);
    }
  }
}

void initBoard()
{
  for (int i = 0; i < TILES; i++)
  {
    for (int j = 0; j < TILES; j++)
    {
      board[i][j].value = 0;
      board[i][j].hidden = false;
      board[i][j].fixed = true;
    }
  }
}

void hideTiles(int numTiles)
{
  int x, y;
  do
  {
    x = randValue(0, 8);
    y = randValue(0, 8);
    if (board[y][x].hidden == false)
    {
      --numTiles;
    }
    board[y][x].hidden = true;
    board[y][x].fixed = false;
  } while (numTiles > 0);
}

void push(Stack **stack, Action action)
{
  Stack *new_node = (Stack *)malloc(sizeof(Stack));
  if (new_node == NULL)
  {
    printf("Error: no se pudo asignar memoria\n");
    exit(1);
  }

  // Evitar repetir la misma acción multiples veces.
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

void moveStacks(Stack **stack1, Stack **stack2, PossibleMoves move)
{
  if (*stack1 == NULL)
  {
    return;
  }
  Action action = pop(stack1);

  switch (move)
  {
  case UNDO:
    board[(int)action.position.x][(int)action.position.y].value = action.oldValue;
    board[(int)action.position.x][(int)action.position.y].hidden = action.oldHidden;
    currentTile.x = action.position.x;
    currentTile.y = action.position.y;
    break;
  case REDO:
    board[(int)action.position.x][(int)action.position.y].value = action.newValue;
    board[(int)action.position.x][(int)action.position.y].hidden = action.newHidden;
    currentTile.x = action.position.x;
    currentTile.y = action.position.y;
    break;
  }

  push(stack2, action);
}

void undo(Stack **undo_stack, Stack **redo_stack)
{
  moveStacks(undo_stack, redo_stack, UNDO);
}

void redo(Stack **undo_stack, Stack **redo_stack)
{
  moveStacks(redo_stack, undo_stack, REDO);
}

void printValues(Stack *stack)
{
  while (stack != NULL)
  {
    // printf("%d\n", stack->action);
    stack = stack->next;
  }
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

void initGame()
{
  initTime();
  initBoard();
  solver();
  hideTiles(EASY);
  printBoard();
}
