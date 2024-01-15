#pragma once

#include "framework.h"
#include "GraphmationForge.h"
#include "GraphDrawer.h"
#include "Timer.h"
#include "PropertiesWindow.h"

#include <unordered_map>
#include <vector>

class ISelectable;
class Node;
class Transition;

class GraphmationForgeApp
{
public:
    static GraphmationForgeApp* const GetInstance() { return s_instance; }
private:
    static GraphmationForgeApp* s_instance;

public:
    GraphmationForgeApp();

    // Win32 message handlers
    int OnWindowCreated(WIN32_CALLBACK_PARAMS);
    int OnPaintCustomBackground(WIN32_CALLBACK_PARAMS);
    int OnWindowPaint(WIN32_CALLBACK_PARAMS);
    int OnWindowCommand(WIN32_CALLBACK_PARAMS);
    int OnLeftMouseButtonDown(WIN32_CALLBACK_PARAMS);
    int OnLeftMouseButtonUp(WIN32_CALLBACK_PARAMS);
    int OnMouseMoved(WIN32_CALLBACK_PARAMS);
    int OnOpenContextMenu(WIN32_CALLBACK_PARAMS);
    int OnWindowResize(WIN32_CALLBACK_PARAMS);
    int OnWindowClosed(WIN32_CALLBACK_PARAMS);

    void SetInstanceHandle(HINSTANCE const instanceHandle) { m_instanceHandle = instanceHandle; }
    bool InitInstance(int cmdShow);

    void LoadStringResources();
    void CreateBrushPalette();
    void CreateFonts();
    void RegisterWindowClasses();

    Node* const CreateNode();
    Node* const CreateNodeAtMousePos();

    Transition* const CreateTransition(unsigned int const fromNodeID, unsigned int const toNodeID);
    Transition* const CreateTransition(Node* const fromNode, Node* const toNode);

    void InvalidateAttachedTransitions(std::vector<ISelectable*> const& selectedObjects);
    bool const AreNodesTwoWayConnected(Node const* const nodeA, Node const* const nodeB);
    std::vector<Transition*> GetTransitionsAttachedFromNode(Node const* const node);

    int const GetStateID(Node const* const node) const; 
    
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

    // File IO
    bool OpenFile();
    bool SaveFile();
    bool SaveAsFile();

    bool LoadJSON(std::string filepath);
    bool SaveJSON(std::string const& filepath);
    bool TryUnloadGraph();

private:
    // Win32
    HINSTANCE m_instanceHandle;
    HWND m_mainWindowHandle;

    PropertiesWindow m_propertiesWindow;

    std::unordered_map<int, WCHAR[MAX_LOADSTRING]> m_stringResources;

    std::vector<HBRUSH> m_brushes;
    std::vector<HFONT> m_fonts;

    std::vector<Node*> m_nodes;
    std::vector<Transition*> m_transitions;

    // The selectable that is currently under the user's mouse
    ISelectable* m_potentialSelectable = nullptr;
    std::vector<ISelectable*> m_selectedObjects;

    // File System
    std::string m_loadedFilepath = "";
    bool m_containsUnsavedChanges = false;

    // Temp?
    TimePoint m_lastUpdateTime = 0;
    bool m_isDragging = false;
};