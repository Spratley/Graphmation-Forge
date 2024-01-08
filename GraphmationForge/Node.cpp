#include "Node.h"

#define REDRAW_DELTA 10

void Node::SetPosition(HWND& hWnd, Point p)
{
    Point deltaPos = p - m_position;
    if (abs(deltaPos.X) > 3 || abs(deltaPos.Y) > 3)
    {
        m_invalidPosition = m_position;
        m_position = p;
        InvalidateScreen(hWnd);
    }
}

void Node::InvalidateScreen(HWND& hWnd)
{
    int overdraw = 3;

    RECT invalidArea;
    invalidArea.left    = m_invalidPosition.X - overdraw;
    invalidArea.top     = m_invalidPosition.Y - overdraw;
    invalidArea.right   = m_invalidPosition.X + NODE_WIDTH + overdraw;
    invalidArea.bottom  = m_invalidPosition.Y + NODE_HEIGHT + overdraw;
    InvalidateRect(hWnd, &invalidArea, false);

    RECT newDrawArea;
    newDrawArea.left    = m_position.X - overdraw;
    newDrawArea.top     = m_position.Y - overdraw;
    newDrawArea.right   = m_position.X + NODE_WIDTH + overdraw;
    newDrawArea.bottom  = m_position.Y + NODE_HEIGHT + overdraw;
    InvalidateRect(hWnd, &newDrawArea, false);
}