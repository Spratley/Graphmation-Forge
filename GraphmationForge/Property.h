#pragma once

#include <string>
#include <vector>

enum PropertyType
{
    TEXT_BOX,
    DROPDOWN
};

struct Property
{
    virtual PropertyType const GetPropertyType() const = 0;
    virtual std::vector<std::wstring> const GetDropdownItems() const { return {}; }
};

struct StringProperty : public Property
{
    StringProperty(std::wstring const& value) : m_value(value) {}

    PropertyType const GetPropertyType() const override { return TEXT_BOX; }

    std::wstring m_value;
};

struct BoolProperty : public Property
{
    BoolProperty(bool const value) : m_value(value) {}

    PropertyType const GetPropertyType() const override { return DROPDOWN; }
    std::vector<std::wstring> const GetDropdownItems() const override { return { L"True", L"False" }; }

    bool m_value;
};