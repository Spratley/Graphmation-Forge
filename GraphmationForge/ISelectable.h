#pragma once

#include "IPaintable.h"

class ISelectable : public IPaintable
{
public:
    ISelectable(HWND const windowHandle) : IPaintable(windowHandle) {}

    void SetSelectionState(SelectionState const state);
    SelectionState const& GetSelectionState() const { return m_selectionState; }

protected:
    SelectionState m_selectionState;
};