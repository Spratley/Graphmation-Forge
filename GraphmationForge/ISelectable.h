#pragma once

#include "IPaintable.h"
#include "PropertyContainer.h"

class ISelectable : public IPaintable
{
public:
    ISelectable(HWND const windowHandle) : IPaintable(windowHandle) {}

    void SetSelectionState(SelectionState const state);
    SelectionState const& GetSelectionState() const { return m_selectionState; }

    std::unordered_map<int, Property*> const& GetProperties() const { return m_properties.GetProperties(); }

protected:
    SelectionState m_selectionState;
    PropertyContainer m_properties;
};