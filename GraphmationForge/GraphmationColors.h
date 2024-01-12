#pragma once
#include "framework.h"

#define GRAPHMATION_COLOR(name, rgb) ID_COLOR_ ## name ,
enum ColorIDs
{
#include "GraphmationColors.inl"
    ID_COLOR_COUNT
};
#undef GRAPHMATION_COLOR

#define GRAPHMATION_COLOR(name, rgb) RGB ## rgb ,
COLORREF g_colors[ID_COLOR_COUNT + 1] = 
{
#include "GraphmationColors.inl"
    RGB(0, 0, 0)
};
#undef GRAPHMATION_COLOR