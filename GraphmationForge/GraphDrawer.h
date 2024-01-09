//#pragma once
//
//#include "framework.h"
//
//class GraphmationForgeApp;
//
//struct Node;
//
//class GraphDrawer
//{
//public:
//    GraphDrawer(GraphmationForgeApp& owner);
//
//    void Draw(HDC& hdc, HWND const& hWnd);
//
//private:
//    void DrawBackground(Graphics& graphics, HWND const& hWnd);
//
//    void DrawRoundedRect(Graphics& graphics, Rect const& rect, float const cornerRadius, bool isSelected);
//    void DrawArrow(Graphics& graphics, Point const& start, Point const& end, bool const isSelected);
//
//    void DrawNode(Graphics& graphics, Node const& node);
//
//private:
//    GraphmationForgeApp& m_owner;
//};