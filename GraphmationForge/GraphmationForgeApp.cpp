#include "GraphmationForgeApp.h"

GraphmationForgeApp::GraphmationForgeApp()
: m_graphDrawer(*this)
{}

void GraphmationForgeApp::Update(HWND& hWnd)
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);
    Point clientMousePos(p.x, p.y);

    temp_node.SetPosition(hWnd, Point(100, 300));

    if (temp_node.IsMouseOverlapping(clientMousePos))
    {
        temp_node.SetSelected(hWnd, true);
    }
    else
    {
        temp_node.SetSelected(hWnd, false);
    }
}

Gdiplus::Point const GraphmationForgeApp::GetScreenDimensions(HWND const& hWnd) const
{
    Point result;
    RECT windowRect;
    if (GetClientRect(hWnd, &windowRect))
    {
        result.X = windowRect.right - windowRect.left;
        result.Y = windowRect.bottom - windowRect.top;
    }
    return result;
}
