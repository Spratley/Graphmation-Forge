#pragma once

#include "framework.h"
#include <string>

#define ENUM_TO_STRING_IMPL(enumItem) case enumItem ## : { return L ## #enumItem ; }
#define ENUM_FROM_STRING_IMPL(enumItem) if (StrCmpW(cString, L ## #enumItem) == 0) { return enumItem; }

struct VariableType
{
    enum Enum
    {
        TYPE_BOOL,
        TYPE_FLOAT,
        TYPE_INT,
        COUNT
    };

    static inline Enum const FromString(std::wstring const& string)
    {
        LPCTSTR cString = string.c_str();
        ENUM_FROM_STRING_IMPL(TYPE_BOOL)
        ENUM_FROM_STRING_IMPL(TYPE_FLOAT)
        ENUM_FROM_STRING_IMPL(TYPE_INT)
        return TYPE_BOOL;
    }

    static inline std::wstring const ToString(Enum const& e)
    {
        switch (e)
        {
        default:
        ENUM_TO_STRING_IMPL(TYPE_BOOL)
        ENUM_TO_STRING_IMPL(TYPE_FLOAT)
        ENUM_TO_STRING_IMPL(TYPE_INT)
        }
    }
};

struct OperatorType
{
    enum Enum
    {
        EQUAL,
        NOT_EQUAL,
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        COUNT
    };

    
    static inline Enum const FromString(std::wstring const& string)
    {
        LPCTSTR cString = string.c_str();
        ENUM_FROM_STRING_IMPL(EQUAL)
        ENUM_FROM_STRING_IMPL(NOT_EQUAL)
        ENUM_FROM_STRING_IMPL(LESS)
        ENUM_FROM_STRING_IMPL(LESS_EQUAL)
        ENUM_FROM_STRING_IMPL(GREATER)
        ENUM_FROM_STRING_IMPL(GREATER_EQUAL)
        return EQUAL;
    }

    static inline std::wstring const ToString(Enum const& e)
    {
        switch (e)
        {
        default:
        ENUM_TO_STRING_IMPL(EQUAL)
        ENUM_TO_STRING_IMPL(NOT_EQUAL)
        ENUM_TO_STRING_IMPL(LESS)
        ENUM_TO_STRING_IMPL(LESS_EQUAL)
        ENUM_TO_STRING_IMPL(GREATER)
        ENUM_TO_STRING_IMPL(GREATER_EQUAL)
        }
    }
};