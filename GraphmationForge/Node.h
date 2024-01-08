#pragma once

#include "framework.h"

struct Node
{
    void SetPosition(HWND& hWnd, Point p);
    void InvalidateScreen(HWND& hWnd);

    Point m_position;
    Point m_invalidPosition;
};