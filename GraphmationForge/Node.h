//#pragma once
//
//#include "framework.h"
//#include <string>
//
//struct Node
//{
//    void SetPosition(HWND& hWnd, Point p);
//    void InvalidateScreen(HWND& hWnd);
//    void InvalidateCurrentPosition(HWND& hWnd);
//
//    bool IsMouseOverlapping(Point mousePos);
//
//    void SetSelected(HWND hWnd, bool const selected);
//
//    Point m_position;
//    Point m_invalidPosition;
//    std::string m_nodeName;
//    std::string m_animation;
//
//    bool m_isSelected = false;
//};

#include "framework.h"

struct Node
{
public:
    void SetPosition(POINT p);
    void InvalidateNodeArea(HWND& parentWindowHandle);

    bool IsMouseOverlapping(POINT mousePos);

    void SetSelected(HWND hWnd, bool const selected);

private:
    HWND m_windowHandle;
};