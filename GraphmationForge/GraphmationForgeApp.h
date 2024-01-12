#pragma once

#include "framework.h"
#include "GraphmationForge.h"
#include "GraphDrawer.h"

#include <unordered_map>
#include <vector>

#define WIN32_CALLBACK_PARAMS HWND const hWnd, UINT const message, WPARAM const wParam, LPARAM const lParam

class ISelectable;
class Node;

class GraphmationForgeApp
{
public:
    static GraphmationForgeApp* const GetInstance() { return s_instance; }
private:
    static GraphmationForgeApp* s_instance;

public:
    GraphmationForgeApp();

    void Update();

    // Win32 message handlers
    int OnWindowCreated(WIN32_CALLBACK_PARAMS);
    int OnPaintCustomBackground(WIN32_CALLBACK_PARAMS);
    int OnWindowPaint(WIN32_CALLBACK_PARAMS);
    int OnWindowCommand(WIN32_CALLBACK_PARAMS);
    int OnLeftMouseButtonDown(WIN32_CALLBACK_PARAMS);
    int OnLeftMouseButtonUp(WIN32_CALLBACK_PARAMS);

    void SetInstanceHandle(HINSTANCE const instanceHandle) { m_instanceHandle = instanceHandle; }
    bool InitInstance(int cmdShow);

    void LoadStringResources();
    void CreateBrushPalette();
    void CreateFonts();
    void RegisterWindowClasses();

    Node* const CreateNode();
    
private:
    void LoadStringResource(int resourceID);
    void CreateBrush(int brushID, COLORREF brushColor);
    void CreateWindowFont(const wchar_t* fontName, int const fontSize, int const fontWeight);
    ATOM RegisterWindowClass(LPCWSTR className, HBRUSH backgroundBrush);

    // Creators
    void OnMainWindowCreated(WIN32_CALLBACK_PARAMS);

    // Paint Functions
    void PaintMainWindow(WIN32_CALLBACK_PARAMS);
    void PaintNode(WIN32_CALLBACK_PARAMS);
    int PaintNodeBackground(WIN32_CALLBACK_PARAMS);

    Node* const FindNode(HWND const nodeWindowHandle) const;

    void DeselectAll();

private:
    // Win32
    HINSTANCE m_instanceHandle;
    HWND m_mainWindowHandle;

    std::unordered_map<int, WCHAR[MAX_LOADSTRING]> m_stringResources;

    std::vector<HBRUSH> m_brushes;
    std::vector<HFONT> m_fonts;

    std::vector<Node*> m_nodes;

    // The selectable that is currently under the user's mouse
    ISelectable* m_potentialSelectable = nullptr;
    std::vector<ISelectable*> m_selectedObjects;

    // Temp?
    bool m_isDragging = false;
};