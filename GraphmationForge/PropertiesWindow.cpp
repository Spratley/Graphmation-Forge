#include "PropertiesWindow.h"

#include "ISelectable.h"

#include "resource.h"

PropertiesWindow::PropertiesWindow(HWND hWnd, HWND parentHandle)
: m_windowHandle(hWnd)
, m_parentHandle(parentHandle)
{
    UpdatePropertiesWindowSize();
    SetPropertiesContent(nullptr);
}

void PropertiesWindow::UpdatePropertiesWindowSize()
{
    RECT parentRect;
    GetClientRect(m_parentHandle, &parentRect);
    MoveWindow(m_windowHandle, parentRect.right - PROPERTIES_PANEL_WIDTH, 0, PROPERTIES_PANEL_WIDTH, parentRect.bottom, true);
}

void PropertiesWindow::SetPropertiesContent(ISelectable * selectedObject)
{
    m_contents.push_back(CreateContent(L"Bruh", TEXT_BOX));
}

void PropertiesWindow::ClearPropertiesContent()
{

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

HWND PropertiesWindow::CreateContent(std::wstring const & label, PropertyType propertyType)
{
    HWND contentWindow =
        CreateWindow(L"EDIT",
            L"EditField",
            WS_CHILD | WS_VISIBLE,
            10, 0, PROPERTIES_PANEL_WIDTH - 20, 30,
            m_windowHandle,
            (HMENU)ID_COMMAND_EDIT, NULL, NULL);

    return contentWindow;
}
