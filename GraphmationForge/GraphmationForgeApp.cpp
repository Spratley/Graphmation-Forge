#include "GraphmationForgeApp.h"

GraphmationForgeApp::GraphmationForgeApp()
: m_graphDrawer(*this)
{}

void GraphmationForgeApp::Update(HWND& hWnd)
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(hWnd, &p);

    temp_node.SetPosition(hWnd, Point(p.x, p.y));
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
