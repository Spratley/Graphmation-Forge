#include "PropertiesWindow.h"

#include "ISelectable.h"

#include "resource.h"

PropertiesWindow::PropertiesWindow(HWND hWnd, HWND parentHandle)
: m_windowHandle(hWnd)
, m_parentHandle(parentHandle)
{
    UpdatePropertiesWindowSize();
}

void PropertiesWindow::UpdatePropertiesWindowSize()
{
    RECT parentRect;
    GetClientRect(m_parentHandle, &parentRect);
    MoveWindow(m_windowHandle, parentRect.right - PROPERTIES_PANEL_WIDTH, 0, PROPERTIES_PANEL_WIDTH, parentRect.bottom, true);
}

void PropertiesWindow::SetPropertiesContent(ISelectable * selectedObject)
{
    m_selectedObject = selectedObject;
    
    int contentOffset = 50;
    std::unordered_map<int, Property*> const& selectedObjectProperties = selectedObject->GetProperties();
    for (auto const& property : selectedObjectProperties)
    {
        Property* propertyPtr = property.second;
        PropertyType propertyType = propertyPtr->GetPropertyType();
        HWND propertyWindowHandle = CreateContent(L"Property", propertyType, contentOffset);
        contentOffset += 30;

        // Initialize values
        switch (propertyType)
        {
        case DROPDOWN:
            {
                // Set potential values in dropdown
                std::vector <std::wstring> dropdownValues = propertyPtr->GetDropdownItems();
                for (std::wstring const& dropdownItem : dropdownValues)
                {
                    SendMessage(propertyWindowHandle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)dropdownItem.c_str());
                }
            }
            break;
        }
    }
}

void PropertiesWindow::ClearPropertiesContent()
{
    m_selectedObject = nullptr;

    for (HWND hWnd : m_contents)
    {
        DestroyWindow(hWnd);
    }
    m_contents.clear();
}

void PropertiesWindow::Paint(WIN32_CALLBACK_PARAMS)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    EndPaint(hWnd, &ps);
}

void PropertiesWindow::PropagatePropertyValues()
{
    // Set the properties of whatever is selected to the values in my content 
}

HWND PropertiesWindow::CreateContent(std::wstring const & label, PropertyType propertyType, int const verticalOffset)
{
    HWND contentWindow = NULL;

    if (propertyType == TEXT_BOX)
    {
        contentWindow = CreateWindow(WC_EDIT,
                L"EditField",
                WS_CHILD | WS_VISIBLE,
                10, verticalOffset, PROPERTIES_PANEL_WIDTH - 20, 30,
                m_windowHandle,
                (HMENU)ID_COMMAND_EDIT, NULL, NULL);
    }
    else if (propertyType == DROPDOWN)
    {
        contentWindow = CreateWindow(WC_COMBOBOX,
            L"Dropdown Field",
            WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_HASSTRINGS,
            10, verticalOffset, PROPERTIES_PANEL_WIDTH - 20, 500,
            m_windowHandle,
            (HMENU)ID_COMMAND_DROPDOWN, NULL, NULL);
    }
    
    m_contents.push_back(contentWindow);
    return contentWindow;
}
