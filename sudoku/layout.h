#pragma once
#include <raylib.h>

typedef struct
{
    float boardSize;
    int boardEnd;
    float tileSize;
    float halfTileSize;
    float halfNumTileSize;
    Vector2 hudSize;
    Vector2 numpadButtonSize;
} RenderLayout;
