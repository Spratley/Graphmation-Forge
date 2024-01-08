#pragma once

#include "framework.h"
#include "GraphmationForge.h"
#include "GraphDrawer.h"

#include "Node.h"

class GraphmationForgeApp
{
public:
    GraphmationForgeApp();

    void Update(HWND& hWnd);

    void Draw(HDC& hdc, HWND const& hWnd) { m_graphDrawer.Draw(hdc, hWnd); }

    Point const GetScreenDimensions(HWND const& hWnd) const;

    Node temp_node;
    bool temp_selected = false;

private:
    GraphDrawer m_graphDrawer;
};