#pragma once
#include "framework.h"

class IPaintable
{
public:
    IPaintable(HWND parentWindowHandle) : m_parentWindowHandle(parentWindowHandle) {}

    void InvalidatePaintArea();
    virtual RECT const GetPaintRect() const = 0;

protected:
    HWND m_parentWindowHandle;

};