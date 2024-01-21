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

PropertiesWindow::~PropertiesWindow()
{
    ClearPropertiesContent();
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
        HWND propertyWindowHandle = CreateContentWindow(L"Property", propertyType, contentOffset);
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

            SendMessage(propertyWindowHandle, (UINT)CB_SETCURSEL, (WPARAM)propertyPtr->GetDropdownDefaultItem(), (LPARAM)0);
            break;
        }
        case TEXT_BOX:
        {
            SetWindowText(propertyWindowHandle, propertyPtr->GetTextItem().c_str());
            break;
        }
        }

        // Add the new content to my memory
        ContentProperty content;
        content.m_windowHandle = propertyWindowHandle;
        content.m_connectedProperty = propertyPtr;
        m_contents.push_back(content);
    }
}

void PropertiesWindow::ClearPropertiesContent()
{
    m_selectedObject = nullptr;

    for (ContentProperty content : m_contents)
    {
        DestroyWindow(content.m_windowHandle);
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
    for (ContentProperty const& content : m_contents)
    {
        HWND contentWindowHandle = content.m_windowHandle;
        std::vector<WCHAR> value;
        switch (content.m_connectedProperty->GetPropertyType())
        {
        case DROPDOWN:
        {
            int const chosenIndex = (int)SendMessage(contentWindowHandle, CB_GETCURSEL, NULL, NULL);
            if (chosenIndex == CB_ERR)
            {
                continue;
            }
            int const length = (int)SendMessage(contentWindowHandle, CB_GETLBTEXTLEN, (WPARAM)chosenIndex, NULL);
            if (length == CB_ERR)
            {
                continue;
            }
            value.resize(length + 1);
            SendMessageW(contentWindowHandle, CB_GETLBTEXT, (WPARAM)chosenIndex, (LPARAM)value.data());
            break;
        }
        case TEXT_BOX:
        {
            int const length = GetWindowTextLength(contentWindowHandle);
            value.resize(length + 1);
            GetWindowText(contentWindowHandle, value.data(), length + 1);
            break;
        }
        }

        content.m_connectedProperty->SetFromText(value.data());
    }
    m_selectedObject->InvalidatePaintArea();
}

HWND PropertiesWindow::CreateContentWindow(std::wstring const & label, PropertyType propertyType, int const verticalOffset)
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
    
    return contentWindow;
}
