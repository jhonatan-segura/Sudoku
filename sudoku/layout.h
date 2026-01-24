#pragma once
#include <raylib.h>

typedef struct
{
    float boardSize;
    int boardEnd;
    float tileSize;
    float halfTileSize;
    float halfNumTileSize;
    float halfActionButtonSize;
    Vector2 hudSize;
    Vector2 numpadButtonSize;
    Vector2 actionButtonSize;
} RenderLayout;
