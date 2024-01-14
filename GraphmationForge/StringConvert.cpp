#include "StringConvert.h"

std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> StringConvert::converter;

std::string StringConvert::ToStr(std::wstring input)
{
    return converter.to_bytes(input);
}

std::wstring StringConvert::ToWStr(std::string input)
{
    return converter.from_bytes(input);
}
