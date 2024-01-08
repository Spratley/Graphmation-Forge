#pragma once

#include "framework.h"

class GraphmationForgeApp;

class GraphDrawer
{
public:
    GraphDrawer(GraphmationForgeApp& owner);

    void Draw(HDC& hdc, HWND const& hWnd);

private:
    void DrawBackground(Graphics& graphics, HWND const& hWnd);

    void DrawRoundedRect(Graphics& graphics, Rect const& rect, float const cornerRadius);

private:
    GraphmationForgeApp& m_owner;
};