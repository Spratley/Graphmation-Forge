#pragma once

#include "framework.h"

#include <vector>

class ISelectable;

enum PropertyType
{
    TEXT_BOX,
    DROPDOWN
};

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

private:
    HWND CreateContent(std::wstring const& label, PropertyType propertyType);

private:
    HWND m_parentHandle;
    HWND m_windowHandle;

    std::vector<HWND> m_contents;
};