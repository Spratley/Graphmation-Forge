#include "IPaintable.h"

void IPaintable::InvalidatePaintArea()
{
    HRGN const& paintRegion = GetPaintRegion();
    InvalidateRgn(m_parentWindowHandle, paintRegion, false);
}