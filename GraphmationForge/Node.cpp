#include "Node.h"
//#include "Node.h"
//
//#define REDRAW_DELTA 10
//
//void Node::SetPosition(HWND& hWnd, Point p)
//{
//    Point deltaPos = p - m_position;
//    if (abs(deltaPos.X) > 3 || abs(deltaPos.Y) > 3)
//    {
//        m_invalidPosition = m_position;
//        m_position = p;
//        InvalidateScreen(hWnd);
//    }
//}
//
//void Node::InvalidateScreen(HWND& hWnd)
//{
//    int overdraw = 3;
//
//    RECT invalidArea;
//    invalidArea.left    = m_invalidPosition.X - overdraw;
//    invalidArea.top     = m_invalidPosition.Y - overdraw;
//    invalidArea.right   = m_invalidPosition.X + NODE_WIDTH + overdraw;
//    invalidArea.bottom  = m_invalidPosition.Y + NODE_HEIGHT + overdraw;
//    InvalidateRect(hWnd, &invalidArea, false);
//
//    RECT newDrawArea;
//    newDrawArea.left    = m_position.X - overdraw;
//    newDrawArea.top     = m_position.Y - overdraw;
//    newDrawArea.right   = m_position.X + NODE_WIDTH + overdraw;
//    newDrawArea.bottom  = m_position.Y + NODE_HEIGHT + overdraw;
//    InvalidateRect(hWnd, &newDrawArea, false);
//}
//
//void Node::InvalidateCurrentPosition(HWND & hWnd)
//{
//    int overdraw = 3;
//
//    RECT newDrawArea;
//    newDrawArea.left = m_position.X - overdraw;
//    newDrawArea.top = m_position.Y - overdraw;
//    newDrawArea.right = m_position.X + NODE_WIDTH + overdraw;
//    newDrawArea.bottom = m_position.Y + NODE_HEIGHT + overdraw;
//    InvalidateRect(hWnd, &newDrawArea, false);
//}
//
//bool Node::IsMouseOverlapping(Point mousePos)
//{
//    Rect nodeRect(m_position.X, m_position.Y, NODE_WIDTH, NODE_HEIGHT);
//
//    if (mousePos.X < nodeRect.GetLeft() || mousePos.X > nodeRect.GetRight())
//    {
//        return false;
//    }
//
//    if (mousePos.Y < nodeRect.GetTop() || mousePos.Y > nodeRect.GetBottom())
//    {
//        return false;
//    }
//    return true;
//}
//
//void Node::SetSelected(HWND hWnd, bool const selected)
//{
//    if (selected != m_isSelected)
//    {
//        m_isSelected = selected;
//        InvalidateCurrentPosition(hWnd);
//    }
//}

void Node::SetPosition(HWND& parentWindowHandle, POINT const& p)
{
    InvalidateNodeArea(parentWindowHandle);
    m_position = p;
    InvalidateNodeArea(parentWindowHandle);
}

void Node::InvalidateNodeArea(HWND & parentWindowHandle)
{
    RECT newDrawArea;
    newDrawArea.left = m_position.x;
    newDrawArea.top = m_position.y;
    newDrawArea.right = m_position.x + NODE_WIDTH;
    newDrawArea.bottom = m_position.y + NODE_HEIGHT;
    InvalidateRect(parentWindowHandle, &newDrawArea, false);
}

bool Node::IsMouseOverlapping(POINT mousePos)
{
    RECT nodeRect;
    GetClientRect(m_windowHandle, &nodeRect);
    
    if (mousePos.x < nodeRect.left || mousePos.x > nodeRect.right)
    {
        return false;
    }

    if (mousePos.y < nodeRect.top || mousePos.y > nodeRect.bottom)
    {
        return false;
    }
    return true;
}

void Node::SetSelected(HWND hWnd, bool const selected)
{
    // TODO: Implement this
}
