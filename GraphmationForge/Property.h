#pragma once

struct Property
{

};

struct StringProperty : public Property
{
    StringProperty(std::wstring const& value) : m_value(value) {}

    std::wstring m_value;
};

struct BoolProperty : public Property
{
    BoolProperty(bool const value) : m_value(value) {}

    bool m_value;
};