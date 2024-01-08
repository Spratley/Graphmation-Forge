// Entry point for application

#include "GraphmationForgeApp.h"
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // Current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // Main window class name
ULONG_PTR gdiplusToken;                         // Pointer to the GDIPlus instance
GraphmationForgeApp app;

HWND hWndGraphArea;
HWND hWndPropertiesWindow;

// Forward declarations of functions included in this code module:
ATOM                RegisterWindowClass(HINSTANCE hInstance, LPCWSTR className);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_GRAPHMATIONFORGE, szWindowClass, MAX_LOADSTRING);

    RegisterWindowClass(hInstance, szWindowClass);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GRAPHMATIONFORGE));

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        app.Update(msg.hwnd);
    }

    GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}

ATOM RegisterWindowClass(HINSTANCE hInstance, LPCWSTR className)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GRAPHMATIONFORGE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GRAPHMATIONFORGE);
    wcex.lpszClassName  = className;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   GdiplusStartupInput gdiplusStartupInput;
   GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

BOOL CALLBACK EnumChildProc(HWND hWndChild, LPARAM lParam)
{
    LPRECT rcParent;
    int idChild;
    
    idChild = GetWindowLong(hWndChild, GWL_ID);

    if (idChild == IDC_C_PROPERTIES || idChild == IDC_C_GRAPHAREA)
    {
        rcParent = (LPRECT)lParam;
        int width = rcParent->right - rcParent->left;
        int height = rcParent->bottom - rcParent->top;

        if (idChild == IDC_C_PROPERTIES)
        {
            MoveWindow(hWndChild, (width / 4) * 3, 0, width / 4, height, TRUE);
        }
        else if (idChild == IDC_C_GRAPHAREA)
        {
            MoveWindow(hWndChild, 0, 0, width / 4 * 3, height, TRUE);
        }
    }

    ShowWindow(hWndChild, SW_SHOW);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // HWND hWndLabel;
    int idChild;

    switch (message)
    {
    case WM_CREATE:
        
        hWndGraphArea = CreateWindow(L"static", L"GraphArea", WS_CHILD | WS_BORDER | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_C_GRAPHAREA, NULL, NULL);
        hWndPropertiesWindow = CreateWindow(L"static", L"Properties", WS_CHILD | WS_BORDER | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_C_PROPERTIES, NULL, NULL);
        
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);

            // Draw graph area
            hdc = BeginPaint(hWndGraphArea, &ps);
            app.Draw(hdc, hWndGraphArea);
            EndPaint(hWndGraphArea, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        EnumChildWindows(hWnd, EnumChildProc, (LPARAM)&clientRect);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}