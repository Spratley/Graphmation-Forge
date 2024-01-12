#include "GraphmationForgeApp.h"

#include "GraphmationColors.h"
#include "Node.h"

// #include <strsafe.h>
#include <windowsx.h>

GraphmationForgeApp* GraphmationForgeApp::s_instance;

// Global callback for Win32
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    GraphmationForgeApp* instance = GraphmationForgeApp::GetInstance();
    int retValue = -1;
    switch (message)
    {
    case WM_CREATE:
        retValue = instance->OnWindowCreated(hWnd, message, wParam, lParam);
        break;
    case WM_ERASEBKGND:
        retValue = instance->OnPaintCustomBackground(hWnd, message, wParam, lParam);
        break;
    case WM_PAINT:
        retValue = instance->OnWindowPaint(hWnd, message, wParam, lParam);
        break;
    case WM_COMMAND:
        retValue = instance->OnWindowCommand(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        retValue = instance->OnLeftMouseButtonDown(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONUP:
        retValue = instance->OnLeftMouseButtonUp(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
        // TODO: CHECK FOR UNSAVED DOCUMENT
        PostQuitMessage(0);
        return 0;
    }

    if (retValue == -1)
    {
        // Window refused to process command, fallback to default behavior
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

GraphmationForgeApp::GraphmationForgeApp()
{
    // Don't care about multiple instances because if there is then you already messed up
    s_instance = this;
}

void GraphmationForgeApp::Update()
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(m_mainWindowHandle, &p);
        
    if (m_isDragging)
    {
        for (ISelectable* selectable : m_selectedObjects)
        {
            // TODO: Don't assume node
            Node* node = static_cast<Node*>(selectable);
            node->SetDragged(p);
        }
        return;
    }

    m_potentialSelectable = nullptr;
    for (Node* const node : m_nodes)
    {
        SelectionState nextMouseOverState = NONE;
        if (node->IsMouseOverlapping(p))
        {
            m_potentialSelectable = node;
            nextMouseOverState = HIGHLIGHTED;
        }

        if (node->GetSelectionState() != SELECTED)
        {
            node->SetSelectionState(nextMouseOverState);
        }
    }
}

int GraphmationForgeApp::OnWindowCreated(WIN32_CALLBACK_PARAMS)
{
    if (hWnd == m_mainWindowHandle)
    {
        OnMainWindowCreated(hWnd, message, wParam, lParam);
        return 0;
    }

    // int id = GetWindowLong(hWnd, GWL_ID);
    // switch (id)
    // {
    // default:
    //     return -1;
    // }

    return 0;
}

int GraphmationForgeApp::OnPaintCustomBackground(WIN32_CALLBACK_PARAMS)
{
    int id = GetWindowLong(hWnd, GWL_ID);
    switch (id)
    {
    case ID_CLASS_NODE:
        return PaintNodeBackground(hWnd, message, wParam, lParam);
    default:
        return -1;
    }

    return 0;
}

int GraphmationForgeApp::OnWindowPaint(WIN32_CALLBACK_PARAMS)
{
    if (hWnd == m_mainWindowHandle)
    {
        PaintMainWindow(hWnd, message, wParam, lParam);
        return 0;
    }

    int id = GetWindowLong(hWnd, GWL_ID);
    switch (id)
    {
    case ID_CLASS_NODE:
        PaintNode(hWnd, message, wParam, lParam);
        break;
    default:
        return -1;
    }

    return 0;
}

int GraphmationForgeApp::OnWindowCommand(WIN32_CALLBACK_PARAMS)
{
    int commandID = LOWORD(wParam);
    // Parse the menu selections:
    switch (commandID)
    {
        // Disable ABOUT menu for now
    // case IDM_ABOUT:
    //     DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
    //     break;
    case IDM_EXIT:
        DestroyWindow(hWnd);
        break;
    case IDM_INSERT_ANIMSTATE:
        CreateNode();
        break;
    default:
        return -1;
    }
    return 0;
}

int GraphmationForgeApp::OnLeftMouseButtonDown(WIN32_CALLBACK_PARAMS)
{   
    // if (hWnd != m_mainWindowHandle)
    // {
    //     // Only interpret mouse button down for the main window
    //     return -1;
    // }

    if (!m_potentialSelectable)
    {
        DeselectAll();
        return 0;
    }

    POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
    if (DragDetect(hWnd, point))
    {
        for (ISelectable* selectable : m_selectedObjects)
        {
            // TODO: Don't assume node
            Node* node = static_cast<Node*>(selectable);
            node->StartDrag(point);
        }

        m_isDragging = true;
    }
    else
    {
        bool found = false;
        for (ISelectable* selectable : m_selectedObjects)
        {
            if (selectable == m_potentialSelectable)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            m_potentialSelectable->SetSelectionState(SELECTED);
            m_selectedObjects.push_back(m_potentialSelectable);
        }
    }

    return 0;
}

int GraphmationForgeApp::OnLeftMouseButtonUp(WIN32_CALLBACK_PARAMS)
{
    // if (hWnd != m_mainWindowHandle)
    // {
    //     // Only interpret mouse button up for the main window
    //     return -1;
    // }

    m_isDragging = false;
    return 0;
}

ATOM GraphmationForgeApp::RegisterWindowClass(LPCWSTR className, HBRUSH backgroundBrush)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_instanceHandle;
    wcex.hIcon = LoadIcon(m_instanceHandle, MAKEINTRESOURCE(IDI_GRAPHMATIONFORGE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = backgroundBrush;
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GRAPHMATIONFORGE);
    wcex.lpszClassName = className;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

void GraphmationForgeApp::OnMainWindowCreated(WIN32_CALLBACK_PARAMS)
{
    // Create all default sub-windows
}

void GraphmationForgeApp::PaintMainWindow(WIN32_CALLBACK_PARAMS)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    EndPaint(hWnd, &ps);
}

void GraphmationForgeApp::PaintNode(WIN32_CALLBACK_PARAMS)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    
    Node* nodeToPaint = FindNode(hWnd);

    RECT textArea;
    textArea.left = 0;
    textArea.top = 0;
    textArea.right = 150;
    textArea.bottom = 50;
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_colors[ID_COLOR_FONT]);
    SelectObject(hdc, m_fonts[0]);

    std::wstring const& nodeName = nodeToPaint->GetNodeName();
    DrawText(hdc, nodeName.c_str(), nodeName.size(), &textArea, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
        
    EndPaint(hWnd, &ps);
}

int GraphmationForgeApp::PaintNodeBackground(WIN32_CALLBACK_PARAMS)
{
    Node* nodeToPaint = FindNode(hWnd);
    // Do nothing if we are deselected, default background is OK
    HBRUSH backgroundColor;
    switch (nodeToPaint->GetSelectionState())
    {
    default:
    case NONE:
        return -1;
    case HIGHLIGHTED:
        backgroundColor = m_brushes[ID_COLOR_NODE_HIGHLIGHTED];
        break;
    case SELECTED:
        backgroundColor = m_brushes[ID_COLOR_NODE_SELECTED];
    }

    HDC hdc = (HDC)wParam;
    RECT nodeRect;
    GetClientRect(hWnd, &nodeRect);
    FillRect(hdc, &nodeRect, backgroundColor);
    return 0;
}

Node * const GraphmationForgeApp::FindNode(HWND const nodeWindowHandle) const
{
    for (Node* const node : m_nodes)
    {
        if (node->GetWindowHandle() == nodeWindowHandle)
        {
            return node;
        }
    }
    return nullptr;
}

void GraphmationForgeApp::DeselectAll()
{
    for (ISelectable* selectable : m_selectedObjects)
    {
        selectable->SetSelectionState(NONE);
    }
    m_selectedObjects.clear();
}

bool GraphmationForgeApp::InitInstance(int cmdShow)
{
    m_mainWindowHandle = CreateWindowW(m_stringResources[IDC_GRAPHMATIONFORGE], m_stringResources[IDS_APP_TITLE], WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, nullptr, m_instanceHandle, nullptr);
           
    if (!m_mainWindowHandle)
    {
        return FALSE;
    }

    ShowWindow(m_mainWindowHandle, cmdShow);
    UpdateWindow(m_mainWindowHandle);

    return TRUE;
}

void GraphmationForgeApp::LoadStringResources()
{
    LoadStringResource(IDS_APP_TITLE);
    LoadStringResource(IDC_GRAPHMATIONFORGE);
    LoadStringResource(ID_CLASS_NODE);
}

void GraphmationForgeApp::CreateBrushPalette()
{
    m_brushes.resize(ID_COLOR_COUNT);
    for (int i = 0; i < ID_COLOR_COUNT; i++)
    {
        CreateBrush(i, g_colors[i]);
    }
}

void GraphmationForgeApp::CreateFonts()
{
    CreateWindowFont(L"Segoe UI", 25, FW_BOLD); // 0
}

void GraphmationForgeApp::RegisterWindowClasses()
{
    RegisterWindowClass(m_stringResources[IDC_GRAPHMATIONFORGE], m_brushes[ID_COLOR_BG]); // Main window
    RegisterWindowClass(m_stringResources[ID_CLASS_NODE], m_brushes[ID_COLOR_NODE_DESELECTED]);
}

Node* const GraphmationForgeApp::CreateNode()
{
    HWND nodeWindowHandle = 
        CreateWindow(m_stringResources[ID_CLASS_NODE],
                     L"Node",
                     WS_CHILD | WS_VISIBLE,
                     10, 10, 150, 50,
                     m_mainWindowHandle,
                     (HMENU)ID_CLASS_NODE, NULL, NULL);
    HRGN region = CreateRoundRectRgn(0, 0, 150, 50, 20, 20);
    SetWindowRgn(nodeWindowHandle, region, true);

    Node* node = new Node(m_mainWindowHandle, nodeWindowHandle);
    m_nodes.push_back(node);

    POINT p;
    p.x = 0;
    p.y = 0;
    node->SetPosition(p);
    return node;
}

void GraphmationForgeApp::LoadStringResource(int resourceID)
{
    LoadStringW(m_instanceHandle, resourceID, m_stringResources[resourceID], MAX_LOADSTRING);
}

void GraphmationForgeApp::CreateBrush(int brushID, COLORREF brushColor)
{
    m_brushes[brushID] = CreateSolidBrush(brushColor);
}

void GraphmationForgeApp::CreateWindowFont(const wchar_t* fontName, int const fontSize, int const fontWeight)
{
    m_fonts.push_back(CreateFont(fontSize, 0, 0, 0, fontWeight, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, fontName));
}
