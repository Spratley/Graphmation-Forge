#include "Node.h"

#include "Property.h"

Node::Node(HWND const parentWindowHandle, HWND const windowHandle)
: ISelectable(parentWindowHandle)
, m_windowHandle(windowHandle)
, m_position()
{
    m_paintRegion = CreateRectRgn(0, 0, NODE_WIDTH, NODE_HEIGHT);
    InitProperties();
}

Node::~Node()
{
    DestroyWindow(m_windowHandle);
}

void Node::SetPosition(POINT const& p)
{
    InvalidatePaintArea();
    m_position = p;
    SetWindowPos(m_windowHandle, HWND_TOP, p.x, p.y, 0, 0, SWP_NOSIZE);
    InvalidatePaintArea();
}

bool Node::IsMouseOverlapping(POINT mousePos)
{
    RECT nodeRect;
    GetClientRect(m_windowHandle, &nodeRect);
    
    if (mousePos.x < (nodeRect.left + m_position.x) || mousePos.x > (nodeRect.right + m_position.x))
    {
        return false;
    }

    if (mousePos.y < (nodeRect.top + m_position.y) || mousePos.y > (nodeRect.bottom + m_position.y))
    {
        return false;
    }
    return true;
}

HRGN const& Node::GetPaintRegion()
{
    RECT paintRect;
    paintRect.left = m_position.x;
    paintRect.top = m_position.y;
    paintRect.right = m_position.x + NODE_WIDTH;
    paintRect.bottom = m_position.y + NODE_HEIGHT;

    SetRectRgn(m_paintRegion, paintRect.left, paintRect.top, paintRect.right, paintRect.bottom);
    return m_paintRegion;
}

void Node::StartDrag(POINT mousePos)
{
    m_dragStartRelativePosition = mousePos;
    m_dragStartPos = m_position;
}

void Node::SetDragged(POINT mousePos)
{
    int deltaDragX = mousePos.x - m_dragStartRelativePosition.x;
    int deltaDragY = mousePos.y - m_dragStartRelativePosition.y;

    POINT newPos;
    newPos.x = m_dragStartPos.x + deltaDragX;
    newPos.y = m_dragStartPos.y + deltaDragY;

    SetPosition(newPos);
}

void Node::SetAnimationName(std::wstring const & animationName)
{
    m_properties.GetPropertyPtr<StringProperty>(PropertyID_AssociatedAnimation)->m_value = animationName;
}

void Node::InitProperties()
{
    m_properties.RegisterProperty(new StringProperty(L"New Node"), PropertyID_NodeName);
    m_properties.RegisterProperty(new StringProperty(L""), PropertyID_AssociatedAnimation);
    m_properties.RegisterProperty(new BoolProperty(false), PropertyID_Loop);
}

void Node::SetNodeName(std::wstring const& name)
{
    m_properties.GetPropertyPtr<StringProperty>(PropertyID_NodeName)->m_value = name;
    InvalidatePaintArea();
}

void Node::SetLoop(bool const loop)
{
    m_properties.GetPropertyPtr<BoolProperty>(PropertyID_Loop)->m_value = loop;
}

std::wstring const & Node::GetNodeName() const
{
    return m_properties.GetPropertyPtr<StringProperty>(PropertyID_NodeName)->m_value;
}

std::wstring const & Node::GetAnimationName() const
{
    return m_properties.GetPropertyPtr<StringProperty>(PropertyID_AssociatedAnimation)->m_value;
}

bool const Node::GetLoop() const
{
    return m_properties.GetPropertyPtr<BoolProperty>(PropertyID_Loop)->m_value;
}
