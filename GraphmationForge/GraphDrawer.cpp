#include "GraphDrawer.h"

#include "GraphmationForgeApp.h"

#define GF_BG_LIGHT Color(52, 73, 94)
#define GF_BG_DARK Color(44, 62, 80)

#define GF_LIGHT_GREY Color(236, 240, 241)
#define GF_LIGHT_TURQUOISE Color(26, 188, 156)
#define GF_DARK_TURQUOISE Color(22, 160, 133)

#define GF_DARK_ORANGE Color(230, 126, 34)

GraphDrawer::GraphDrawer(GraphmationForgeApp& owner)
: m_owner(owner)
{}

void GraphDrawer::Draw(HDC & hdc, HWND const& hWnd)
{
    Graphics graphics(hdc);
    graphics.SetCompositingMode(CompositingModeSourceCopy);
    graphics.SetCompositingQuality(CompositingQualityHighSpeed);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighSpeed);
    graphics.SetSmoothingMode(SmoothingModeHighSpeed);
    graphics.SetInterpolationMode(InterpolationModeLowQuality);

    DrawBackground(graphics, hWnd);

    Point nodePos = m_owner.temp_node.m_position;
    NODE_SIZE;
    Rect nodeRect(nodePos, nodeSize);
    DrawRoundedRect(graphics, nodeRect, 25.0f);

    Pen pen(GF_LIGHT_GREY, 10.0f);
    pen.SetStartCap(LineCap::LineCapDiamondAnchor);
    pen.SetEndCap(LineCap::LineCapArrowAnchor);
    graphics.DrawLine(&pen, Point(20, 10), Point(70, 50));
}

void GraphDrawer::DrawBackground(Graphics & graphics, HWND const & hWnd)
{
    Point screenDimensions = m_owner.GetScreenDimensions(hWnd);

    Rect screenRect(0, 0, screenDimensions.X, screenDimensions.Y);
    LinearGradientBrush gradientBrush(Point(0, 0), Point(0, screenDimensions.Y), GF_BG_LIGHT, GF_BG_DARK);

    graphics.FillRectangle(&gradientBrush, screenRect);
}

void GraphDrawer::DrawRoundedRect(Graphics& graphics, Rect const & rect, float const cornerRadius)
{
    Point gradientTop;
    Point gradientBottom;
    rect.GetLocation(&gradientTop);
    gradientBottom = gradientTop;
    gradientBottom.Y += rect.Height;
    
    int clampedRadius = static_cast<int>(min(min(cornerRadius, rect.Width * 0.5f), rect.Height * 0.5f));
    int clampedDiameter = clampedRadius * 2;

    // Draw fill
    Rect fillVertical(rect.X + clampedRadius, rect.Y, rect.Width - clampedDiameter, rect.Height);
    Rect fillHorizontal(rect.X, rect.Y + clampedRadius, rect.Width, rect.Height - clampedDiameter);

    Rect cornerTL(rect.X, rect.Y, clampedDiameter, clampedDiameter);
    Rect cornerTR(rect.X + rect.Width - clampedDiameter, rect.Y, clampedDiameter, clampedDiameter);
    Rect cornerBL(rect.X, rect.Y + rect.Height - clampedDiameter, clampedDiameter, clampedDiameter);
    Rect cornerBR(rect.X + rect.Width - clampedDiameter, rect.Y + rect.Height - clampedDiameter, clampedDiameter, clampedDiameter);

    LinearGradientBrush nodeBrush(gradientTop, gradientBottom, GF_LIGHT_TURQUOISE, GF_DARK_TURQUOISE);
    graphics.FillRectangle(&nodeBrush, fillVertical);
    graphics.FillRectangle(&nodeBrush, fillHorizontal);

    graphics.FillEllipse(&nodeBrush, cornerTL);
    graphics.FillEllipse(&nodeBrush, cornerTR);
    graphics.FillEllipse(&nodeBrush, cornerBL);
    graphics.FillEllipse(&nodeBrush, cornerBR);

    // Draw outline
    GraphicsPath outline;
    outline.Reset();
    outline.AddArc(cornerTL, 180, 90);
    outline.AddArc(cornerTR, 270, 90);
    outline.AddArc(cornerBR, 0, 90);
    outline.AddArc(cornerBL, 90, 90);
    outline.AddArc(cornerTL, 180, 1); // Close path


    Pen outlinePen(GF_LIGHT_GREY, 4.0f);
    graphics.DrawPath(&outlinePen, &outline);
}