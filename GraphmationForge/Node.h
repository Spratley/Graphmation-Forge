#pragma once
#include "framework.h"

#include "ISelectable.h"

#include <string>

class Node : public ISelectable
{
public:
    Node(HWND const parentWindowHandle, HWND const windowHandle);
    ~Node();

    void SetPosition(POINT const& p);

    HWND const& GetWindowHandle() const { return m_windowHandle; }
    POINT const& GetNodePosition() const { return m_position; }

    bool IsMouseOverlapping(POINT mousePos);

    HRGN const& GetPaintRegion() override;

    void StartDrag(POINT mousePos);
    void SetDragged(POINT mousePos);

    // Properties    
    void SetNodeName(std::wstring const& name);
    void SetAnimationName(std::wstring const& animationName);
    void SetLoop(bool const loop);

    std::wstring const& GetNodeName() const;
    std::wstring const& GetAnimationName() const;
    bool const GetLoop() const;

private:
    void InitProperties();

private:
    HWND m_windowHandle;

    POINT m_position;

    POINT m_dragStartRelativePosition;
    POINT m_dragStartPos;

    // Properties
    // std::wstring m_nodeName;
    // std::wstring m_associatedAnimation;
    // bool m_loop;
    enum
    {
        PropertyID_NodeName,
        PropertyID_AssociatedAnimation,
        PropertyID_Loop
    };
};