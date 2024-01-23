#include "Property.h"

#define PROPERTY_BOOL_TRUE L"True"
#define PROPERTY_BOOL_FALSE L"False"

std::vector<std::wstring> const BoolProperty::GetDropdownItems() const
{
    return
    {
        PROPERTY_BOOL_TRUE,
        PROPERTY_BOOL_FALSE
    };
}

int const BoolProperty::GetDropdownDefaultItem() const
{
    if (m_value)
    {
        return 0;
    }
    return 1;
}

void BoolProperty::SetFromText(LPCTSTR text)
{
    if (StrCmpW(PROPERTY_BOOL_TRUE, text) == 0)
    {
        m_value = true;
    }
    else
    {
        m_value = false;
    }
}

void StringProperty::SetFromText(LPCTSTR text)
{
    m_value = std::wstring(text);
}

void VariableProperty::SetFromText(LPCTSTR text)
{
    if (StrCmpW(L"", text) == 0)
    {
        m_value.m_int = 0;
        return;
    }

    if (StrCmpW(PROPERTY_BOOL_TRUE, text) == 0)
    {
        m_value.m_bool = true;
        return;
    }
    else if (StrCmpW(PROPERTY_BOOL_FALSE, text) == 0)
    {
        m_value.m_bool = false;
        return;
    }

    std::wstring wstrText(text);
    if (wstrText.find(L".") != std::wstring::npos)
    {
        m_value.m_float = std::stof(wstrText);
        return;
    }
    m_value.m_int = std::stoi(wstrText);
}

PropertyType const VariableProperty::GetPropertyType() const
{
    if (m_variableTypeState == VariableType::TYPE_BOOL)
    {
        return DROPDOWN;
    }
    
    return TEXT_BOX;
}

std::vector<std::wstring> const VariableProperty::GetDropdownItems() const
{
    return
    {
        PROPERTY_BOOL_TRUE,
        PROPERTY_BOOL_FALSE
    };
}

std::wstring const VariableProperty::GetTextItem() const
{
    if (m_variableTypeState == VariableType::TYPE_FLOAT)
    {
        return std::to_wstring(m_value.m_float);
    }

    return std::to_wstring(m_value.m_int);
}

void VariableProperty::SetTypeState(VariableType::Enum type)
{
    // HACK idk how to do this better it's midnight and I just want this program done
    if (type == VariableType::TYPE_FLOAT)
    {
        m_value.m_float = (float)m_value.m_int;
    }
    if (type == VariableType::TYPE_BOOL)
    {
        if (m_variableTypeState == VariableType::TYPE_INT)
        {
            m_value.m_bool = m_value.m_int != 0;
        }
        else if (m_variableTypeState == VariableType::TYPE_FLOAT)
        {
            m_value.m_bool = m_value.m_float != 0.0f;
        }
    }
    if (type == VariableType::TYPE_INT && m_variableTypeState == VariableType::TYPE_FLOAT)
    {
        m_value.m_int = (int)m_value.m_float;
    }

    m_variableTypeState = type;
}
