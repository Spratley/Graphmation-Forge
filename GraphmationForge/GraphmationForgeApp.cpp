#include "GraphmationForgeApp.h"
//#include "GraphmationForgeApp.h"
//
//GraphmationForgeApp::GraphmationForgeApp()
//: m_graphDrawer(*this)
//{}
//
//void GraphmationForgeApp::Update(HWND& hWnd)
//{
//    POINT p;
//    GetCursorPos(&p);
//    ScreenToClient(hWnd, &p);
//    Point clientMousePos(p.x, p.y);
//
//    temp_node.SetPosition(hWnd, Point(100, 300));
//
//    if (temp_node.IsMouseOverlapping(clientMousePos))
//    {
//        temp_node.SetSelected(hWnd, true);
//    }
//    else
//    {
//        temp_node.SetSelected(hWnd, false);
//    }
//}
//
//Gdiplus::Point const GraphmationForgeApp::GetScreenDimensions(HWND const& hWnd) const
//{
//    Point result;
//    RECT windowRect;
//    if (GetClientRect(hWnd, &windowRect))
//    {
//        result.X = windowRect.right - windowRect.left;
//        result.Y = windowRect.bottom - windowRect.top;
//    }
//    return result;
//}

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
    case WM_PAINT:
        retValue = instance->OnWindowCreated(hWnd, message, wParam, lParam);
        break;
    case WM_DESTROY:
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

int GraphmationForgeApp::OnWindowCreated(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

int GraphmationForgeApp::OnWindowPaint(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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

bool GraphmationForgeApp::InitInstance(int cmdShow)
{
    HWND hWnd = CreateWindowW(m_stringResources[IDC_GRAPHMATIONFORGE], m_stringResources[IDS_APP_TITLE], WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, m_instanceHandle, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, cmdShow);
    UpdateWindow(hWnd);

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

    CreateBrush(ID_COLOR_BG, COLOR_BG);
    CreateBrush(ID_COLOR_NODE, COLOR_NODE);
    CreateBrush(ID_COLOR_FONT, COLOR_FONT);
}

void GraphmationForgeApp::CreateFonts()
{
    CreateWindowFont(L"Segoe UI", 25, FW_BOLD); // 0
}

void GraphmationForgeApp::RegisterWindowClasses()
{
    RegisterWindowClass(m_stringResources[IDC_GRAPHMATIONFORGE], m_brushes[ID_COLOR_BG]); // Main window
    RegisterWindowClass(m_stringResources[ID_CLASS_NODE], m_brushes[ID_COLOR_NODE]);
}

Node* const GraphmationForgeApp::CreateNode()
{
    return nullptr;
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
