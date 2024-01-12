#pragma once
#include "framework.h"

#include "ISelectable.h"

#include <string>

class Node : public ISelectable
{
public:
    Node(HWND const parentWindowHandle, HWND const windowHandle);

    void SetPosition(POINT const& p);
    void SetNodeName(std::wstring const& name);
    void SetAnimationName(std::wstring const& animationName);

    HWND const& GetWindowHandle() const { return m_windowHandle; }
    POINT const& GetNodePosition() const { return m_position; }
    std::wstring const& GetNodeName() const { return m_nodeName; }
    std::wstring const& GetAnimationName() const { return m_associatedAnimation; }

    bool IsMouseOverlapping(POINT mousePos);

    RECT const GetPaintRect() const override;

    void StartDrag(POINT mousePos);
    void SetDragged(POINT mousePos);

private:
    HWND m_windowHandle;

    POINT m_position;
    std::wstring m_nodeName;
    std::wstring m_associatedAnimation;

    POINT m_dragStartRelativePosition;
};