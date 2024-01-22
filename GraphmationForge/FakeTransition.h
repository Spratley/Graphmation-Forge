#pragma once

#include "IPaintable.h"

// HACK: Duplicate all the functionality to draw a transition out of transition.h
// This is just because at this point I want this app shipped and out of my hair

class FakeTransition : public IPaintable
{
public:
    FakeTransition(HWND const parentWindowHandle);

    HRGN const& GetPaintRegion() override;

    void Paint(HDC hdc, HBRUSH fillColor);


    void SetFromPoint(POINT const& from) { m_from = from; }
    void SetToPoint(POINT const& to) { m_to = to; }

    void ForceInvalidateCurrentPaintRegion();

private:
    void UpdateRegion();

    void BuildPointArrow(POINT* outPoints, float directionX, float directionY);

private:
    POINT m_from;
    POINT m_to;

    HRGN m_paintRegion;
};