#pragma once

#include "framework.h"
#include "GraphmationForge.h"
#include "GraphDrawer.h"

#include <unordered_map>
#include <vector>

struct Node;

class GraphmationForgeApp
{
public:
    static GraphmationForgeApp* const GetInstance() { return s_instance; }
private:
    static GraphmationForgeApp* s_instance;

public:
    GraphmationForgeApp();

    // Win32 message handlers
    int OnWindowCreated(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    int OnWindowPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    void SetInstanceHandle(HINSTANCE const instanceHandle) { m_instanceHandle = instanceHandle; }

    void LoadStringResources();
    void CreateBrushPalette();
    void RegisterWindowClasses();

    Node* const CreateNode();
    
private:
    void LoadStringResource(int resourceID);
    void CreateBrush(int brushID, COLORREF brushColor);
    ATOM RegisterWindowClass(LPCWSTR className, HBRUSH backgroundBrush);

private:
    // Win32
    HINSTANCE m_instanceHandle;

    std::unordered_map<int, WCHAR[MAX_LOADSTRING]> m_stringResources;
    std::vector<HBRUSH> m_brushes;

    std::vector<Node*> m_nodes;
};