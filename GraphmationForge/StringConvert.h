#pragma once
#include <string>

#include <codecvt>
#include <locale>

class StringConvert
{
public:
    static std::string ToStr(std::wstring input);
    static std::wstring ToWStr(std::string input);

private:
    static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
};