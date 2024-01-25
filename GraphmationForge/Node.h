#pragma once
#include "framework.h"

#include "ISelectable.h"

#include <string>

#include "EnumHelper.h"

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
    void SetLoop(bool const loop);

    std::wstring const& GetNodeName() const;
    bool const GetLoop() const;

    NodeType::Enum const& GetNodeType() const { return m_type; }

    virtual void InitProperties();

protected:
    enum
    {
        PropertyID_NodeName,
        PropertyID_Loop,
        NumBaseNodeProperties
    };

protected:
    NodeType::Enum m_type = NodeType::NORMAL;

private:
    HWND m_windowHandle;

    POINT m_position;

    POINT m_dragStartRelativePosition;
    POINT m_dragStartPos;
};