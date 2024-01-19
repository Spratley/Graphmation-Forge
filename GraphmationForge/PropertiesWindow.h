#pragma once

#include "framework.h"

#include "Property.h"

#include <vector>

class ISelectable;

class PropertiesWindow
{
public:
    PropertiesWindow() {}
    PropertiesWindow(HWND hWnd, HWND parentHandle);

    void UpdatePropertiesWindowSize();

    void SetPropertiesContent(ISelectable* selectedObject);
    void ClearPropertiesContent();

    void Paint(WIN32_CALLBACK_PARAMS);

    void PropagatePropertyValues();

    HWND const& GetHWND() { return m_windowHandle; }

private:
    HWND CreateContent(std::wstring const& label, PropertyType propertyType, int const verticalOffsets);

private:
    HWND m_parentHandle;
    HWND m_windowHandle;

    std::vector<HWND> m_contents;
    ISelectable* m_selectedObject = nullptr;
};