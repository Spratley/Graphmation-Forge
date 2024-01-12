#include "Node.h"

Node::Node(HWND const parentWindowHandle, HWND const windowHandle)
: ISelectable(parentWindowHandle)
, m_windowHandle(windowHandle)
, m_position()
, m_nodeName(L"New Node")
, m_associatedAnimation(L"")
{}

void Node::SetPosition(POINT const& p)
{
    InvalidatePaintArea();
    m_position = p;
    InvalidatePaintArea();
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

RECT const Node::GetPaintRect() const
{
    RECT paintRect;
    paintRect.left = m_position.x;
    paintRect.top = m_position.y;
    paintRect.right = m_position.x + NODE_WIDTH;
    paintRect.bottom = m_position.y + NODE_HEIGHT;
    return paintRect;
}

void Node::StartDrag(POINT mousePos)
{
    m_dragStartRelativePosition = mousePos;
}

void Node::SetDragged(POINT mousePos)
{
    int deltaDragX = mousePos.x - m_dragStartRelativePosition.x;
    int deltaDragY = mousePos.y - m_dragStartRelativePosition.y;

    POINT newPos;
    newPos.x = m_position.x + deltaDragX;
    newPos.y = m_position.y + deltaDragY;

    SetPosition(newPos);
}

void Node::SetAnimationName(std::wstring const & animationName)
{
    m_associatedAnimation = animationName;
}

void Node::SetNodeName(std::wstring const& name)
{
    m_nodeName = name;
    InvalidatePaintArea();
}
