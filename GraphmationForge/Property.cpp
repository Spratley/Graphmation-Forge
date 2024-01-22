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
