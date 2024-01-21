#pragma once
#include "framework.h"

#include <string>
#include <vector>

enum PropertyType
{
    TEXT_BOX
    , DROPDOWN
    , COMPOUND // Compound properties contain a series of children properties that all should get displayed
};

union Variable
{
    float m_float;
    int m_int;
    bool m_bool;
};

struct Property
{
    virtual PropertyType const GetPropertyType() const = 0;
    virtual std::vector<std::wstring> const GetDropdownItems() const { return {}; }
    virtual int const GetDropdownDefaultItem() const { return 0; }
    virtual std::wstring const GetTextItem() const { return std::wstring(L""); }

    virtual void SetFromText(LPCTSTR text) = 0;
};

struct StringProperty : public Property
{
    StringProperty(std::wstring const& value) : m_value(value) {}

    PropertyType const GetPropertyType() const override { return TEXT_BOX; }
    std::wstring const GetTextItem() const override { return m_value; }

    void SetFromText(LPCTSTR text) override;

    std::wstring m_value;
};

struct BoolProperty : public Property
{
    BoolProperty(bool const value) : m_value(value) {}

    PropertyType const GetPropertyType() const override { return DROPDOWN; }
    std::vector<std::wstring> const GetDropdownItems() const override;
    int const GetDropdownDefaultItem() const override;

    void SetFromText(LPCTSTR text) override;

    bool m_value;
};

struct VariableProperty : public Property
{
    void SetFromText(LPCTSTR text) override;
    PropertyType const GetPropertyType() const override { return TEXT_BOX; }

    Variable m_value;
};

template <typename T, typename E>
struct EnumProperty : public Property
{
    PropertyType const GetPropertyType() const override { return DROPDOWN; }
    std::vector<std::wstring> const GetDropdownItems() const override;
    int const GetDropdownDefaultItem() const override;

    void SetFromText(LPCTSTR text) override;

    EnumProperty(E const value) : m_value(value) {}

    E m_value;
};

template <typename T>
struct VectorProperty : public Property
{
    PropertyType const GetPropertyType() const override { return COMPOUND; }

    void SetFromText(LPCTSTR text) override {}

    std::vector<T> m_value;
};

// Implementations

template <typename T, typename E>
inline std::vector<std::wstring> const EnumProperty<T, E>::GetDropdownItems() const
{
    std::vector<std::wstring> result;
    for (int i = 0; i < E::COUNT; i++)
    {
        result.push_back(T::ToString((E)i));
    }
    return result;
}

template <typename T, typename E>
inline int const EnumProperty<T, E>::GetDropdownDefaultItem() const
{
    return (int)m_value;
}

template <typename T, typename E>
inline void EnumProperty<T, E>::SetFromText(LPCTSTR text)
{
    m_value = T::FromString(std::wstring(text));
}
