#include "utils.h"

Vec2i getNumPadPosition(int value)
{
  int x = 0;
  int y = 0;
  switch (value)
  {
  case 1:
    x = 0;
    y = 0;
    break;
  case 2:
    x = 0;
    y = 1;
    break;
  case 3:
    x = 0;
    y = 2;
    break;
  case 4:
    x = 1;
    y = 0;
    break;
  case 5:
    x = 1;
    y = 1;
    break;
  case 6:
    x = 1;
    y = 2;
    break;
  case 7:
    x = 2;
    y = 0;
    break;
  case 8:
    x = 2;
    y = 1;
    break;
  case 9:
    x = 2;
    y = 2;
    break;
  default:
    break;
  }

  return (Vec2i) {x, y};
}