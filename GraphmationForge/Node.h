#pragma once
#include "framework.h"
#include <string>

struct Node
{
public:
    Node(HWND const windowHandle);

    void SetPosition(HWND& parentWindowHandle, POINT const& p);
    void InvalidateNodeArea(HWND& parentWindowHandle);

    bool IsMouseOverlapping(POINT mousePos);

    void SetSelectionState(SelectionState const state);

private:
    HWND m_windowHandle;

    POINT m_position;
    std::string m_nodeName;
    std::string m_associatedAnimation;

    SelectionState m_selectionState;
};