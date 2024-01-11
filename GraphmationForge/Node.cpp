#include "Node.h"

Node::Node(HWND const windowHandle)
: m_windowHandle(windowHandle)
, m_position()
, m_nodeName("New Node")
, m_associatedAnimation("")
{}

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

void Node::SetSelectionState(SelectionState const state)
{
    m_selectionState = state;
}
