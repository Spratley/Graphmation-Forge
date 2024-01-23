#include "PropertiesWindow.h"

#include "ISelectable.h"

#include "resource.h"

#include "Transition.h"

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
    AddPropertiesToPanel(selectedObject->GetProperties(), contentOffset);
}

void PropertiesWindow::ClearPropertiesContent()
{
    m_selectedObject = nullptr;

    for (ContentProperty content : m_contents)
    {
        DestroyWindow(content.m_windowHandle);
    }
    m_contents.clear();

    for (HWND hWnd : m_additionalObjectsToRemove)
    {
        DestroyWindow(hWnd);
    }
    m_additionalObjectsToRemove.clear();
}

void PropertiesWindow::RebuildPropertiesContent()
{
    ISelectable* selectedObject = m_selectedObject;
    ClearPropertiesContent();
    SetPropertiesContent(selectedObject);
}

int PropertiesWindow::AddPropertiesToPanel(std::unordered_map<int, std::shared_ptr<Property>> propertiesSet, int contentOffset)
{
    // HACK: Store the type for the condition here, this only works because nothing else uses the variable type enum
    VariableType::Enum selectedConditionType;

    for (auto const& property : propertiesSet)
    {
        std::shared_ptr<Property> propertyPtr = property.second;

        if (std::shared_ptr<EnumProperty<VariableType, VariableType::Enum>> enumProperty = std::dynamic_pointer_cast<EnumProperty<VariableType, VariableType::Enum>>(propertyPtr))
        {
            selectedConditionType = enumProperty->m_value;
        }

        if (std::shared_ptr<VariableProperty> variableProperty = std::dynamic_pointer_cast<VariableProperty>(propertyPtr))
        {
            variableProperty->SetTypeState(selectedConditionType);
        }

        PropertyType propertyType = propertyPtr->GetPropertyType();
        if (propertyType == COMPOUND)
        {
            contentOffset += HandleConditionsProperty(propertyPtr, contentOffset);
            continue;
        }

        HWND propertyWindowHandle = CreateContentWindow(L"Property", propertyType, contentOffset);
        contentOffset += 35;

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

    return contentOffset;
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

void PropertiesWindow::DeleteCondition(int conditionIndex)
{
    Transition* transition = dynamic_cast<Transition*>(m_selectedObject);
    if (!transition)
    {
        return;
    }

    transition->DeleteCondition(conditionIndex);
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

int PropertiesWindow::HandleConditionsProperty(std::shared_ptr<Property> const property, int const contentOffset)
{
    int postOffset = contentOffset;

    std::shared_ptr<VectorProperty<TransitionCondition>> conditionsProperty = std::dynamic_pointer_cast<VectorProperty<TransitionCondition>>(property);
    if (!conditionsProperty)
    {
        return postOffset;
    }

    std::vector<TransitionCondition> const& conditions = conditionsProperty->m_value;

    postOffset += 10;

    // Create fields for all conditions
    int i = 0;
    for (TransitionCondition const& condition : conditions)
    {
        postOffset = AddPropertiesToPanel(condition.m_properties.GetProperties(), postOffset);

        HWND deleteButton = CreateDeleteConditionButton(i);
        m_additionalObjectsToRemove.push_back(deleteButton);
        MoveWindow(deleteButton, 10, postOffset, PROPERTIES_PANEL_WIDTH - 20, 30, true);
        
        postOffset += 45;
        i++;
    }

    // Create ADD button for conditions
    HWND newButton = CreateNewConditionButton();
    m_additionalObjectsToRemove.push_back(newButton);
    MoveWindow(newButton, 10, postOffset, PROPERTIES_PANEL_WIDTH - 20, 30, true);
    postOffset += 35;
    return postOffset;
}

HWND PropertiesWindow::CreateNewConditionButton()
{
    HWND hWndButton = CreateWindow(
        L"BUTTON", L"New Condition", 
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        0, 0, 0, 0,
        m_windowHandle,
        (HMENU)ID_COMMAND_NEW_CONDITION,
        NULL, NULL);

    return hWndButton;
}

HWND PropertiesWindow::CreateDeleteConditionButton(int const conditionIndex)
{
    HWND hWndButton = CreateWindow(
        L"BUTTON", L"Delete Condition",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        0, 0, 0, 0,
        m_windowHandle,
        (HMENU)ID_COMMAND_DELETE_CONDITION + conditionIndex,
        NULL, NULL);
    return hWndButton;
}