#include "IPaintable.h"

void IPaintable::InvalidatePaintArea()
{
    RECT paintRect = GetPaintRect();
    InvalidateRect(m_parentWindowHandle, &paintRect, false);
}
