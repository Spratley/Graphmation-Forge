#pragma once

#include "framework.h"

#include "Property.h"

#include <vector>
#include <unordered_map>

struct Property;

class ISelectable;

struct ContentProperty
{
    HWND m_windowHandle;
    std::shared_ptr<Property> m_connectedProperty;
};

class PropertiesWindow
{
public:
    PropertiesWindow() {}
    PropertiesWindow(HWND hWnd, HWND parentHandle);
    ~PropertiesWindow();

    void UpdatePropertiesWindowSize();

    void SetPropertiesContent(ISelectable* selectedObject);
    void ClearPropertiesContent();
    void RebuildPropertiesContent();

    int AddPropertiesToPanel(std::unordered_map<int, std::shared_ptr<Property>> propertiesSet, int contentOffset);

    void Paint(WIN32_CALLBACK_PARAMS);

    void PropagatePropertyValues();

    HWND const& GetHWND() { return m_windowHandle; }

    static bool const IsDeleteConditionButton(int const commandID) { return commandID >= 300 && commandID < 400; }

    void DeleteCondition(int conditionIndex);

private:
    HWND CreateContentWindow(std::wstring const& label, PropertyType propertyType, int const verticalOffsets);

    int HandleConditionsProperty(std::shared_ptr<Property> const property, int const contentOffset);

    HWND CreateNewConditionButton();
    HWND CreateDeleteConditionButton(int const conditionID);

private:
    HWND m_parentHandle;
    HWND m_windowHandle;

    std::vector<ContentProperty> m_contents;
    std::vector<HWND> m_additionalObjectsToRemove;
    ISelectable* m_selectedObject = nullptr;
};