#pragma once

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

Vec2i getNumPadPosition(int value);