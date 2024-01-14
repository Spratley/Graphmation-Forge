#pragma once
#include "framework.h"

class IPaintable
{
public:
    IPaintable(HWND parentWindowHandle) : m_parentWindowHandle(parentWindowHandle) {}

    void InvalidatePaintArea();
    virtual HRGN const& GetPaintRegion() = 0;

protected:
    HWND m_parentWindowHandle;
    HRGN m_paintRegion;
};