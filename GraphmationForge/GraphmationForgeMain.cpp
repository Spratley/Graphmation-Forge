// Entry point for application

#include "GraphmationForgeApp.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    GraphmationForgeApp app;
    app.SetInstanceHandle(hInstance);
    app.LoadStringResources();
    app.CreateBrushPalette();
    app.CreateFonts();
    app.RegisterWindowClasses();

    if (!app.InitInstance(nCmdShow))
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
    }
    
    return (int) msg.wParam;
}

BOOL CALLBACK EnumChildProc(HWND hWndChild, LPARAM lParam)
{    int idChild;

    
    idChild = GetWindowLong(hWndChild, GWL_ID);

    // if (idChild == IDC_C_PROPERTIES || idChild == IDC_C_GRAPHAREA)
    // {
    //     rcParent = (LPRECT)lParam;
    //     int width = rcParent->right - rcParent->left;
    //     int height = rcParent->bottom - rcParent->top;
    // 
    //     if (idChild == IDC_C_PROPERTIES)
    //     {
    //         MoveWindow(hWndChild, (width / 4) * 3, 0, width / 4, height, TRUE);
    //     }
    //     else if (idChild == IDC_C_GRAPHAREA)
    //     {
    //         MoveWindow(hWndChild, 0, 0, width / 4 * 3, height, TRUE);
    //     }
    // }

    ShowWindow(hWndChild, SW_SHOW);

    return TRUE;
}

// LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     HWND hWndLabel; 
//     HWND hWndnodeLabel;
//     HRGN region;
// 
//     int id;
// 
//     switch (message)
//     {
//     case WM_CREATE:
//         
//         id = GetWindowLong(hWnd, GWL_ID);
//         if (id != 169)
//         {
//             hWndLabel = CreateWindow(szNodeClass, L"TestNode", WS_CHILD | WS_VISIBLE, 10, 10, 150, 50, hWnd, (HMENU)(169), NULL, NULL);
//             region = CreateRoundRectRgn(0, 0, 150, 50, 20, 20);
//             SetWindowRgn(hWndLabel, region, true);
// 
//             hWndnodeLabel = CreateWindow(szNodeClass, L"TestNode2", WS_CHILD | WS_VISIBLE, 460, 302, 150, 50, hWnd, (HMENU)(169), NULL, NULL);
//             region = CreateRoundRectRgn(0, 0, 150, 50, 20, 20);
//             SetWindowRgn(hWndnodeLabel, region, true);
//         }
// 
//         break;
//     case WM_COMMAND:
//         {
//             int wmId = LOWORD(wParam);
//             // Parse the menu selections:
//             switch (wmId)
//             {
//             case IDM_ABOUT:
//                 DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                 break;
//             case IDM_EXIT:
//                 DestroyWindow(hWnd);
//                 break;
//             default:
//                 return DefWindowProc(hWnd, message, wParam, lParam);
//             }
//         }
//         break;
//     case WM_PAINT:
//         {
//             PAINTSTRUCT ps;
//             HDC hdc = BeginPaint(hWnd, &ps);
// 
//             id = GetWindowLong(hWnd, GWL_ID);
//             if (id == 169)
//             {
//                 RECT textArea;
//                 textArea.left = 0;
//                 textArea.top = 0;
//                 textArea.right = 150;
//                 textArea.bottom = 50;
//                 SetBkMode(hdc, TRANSPARENT);
//                 SetTextColor(hdc, COLOR_FONT);
//                 SelectObject(hdc, hFont);
//                 DrawText(hdc, L"Bonk Text", 9, &textArea, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
//             }
// 
//             EndPaint(hWnd, &ps);
//         }
//         break;
//     case WM_DESTROY:
//         PostQuitMessage(0);
//         break;
//     case WM_SIZE:
//         RECT clientRect;
//         GetClientRect(hWnd, &clientRect);
//         EnumChildWindows(hWnd, EnumChildProc, (LPARAM)&clientRect);
//         break; 
//     case WM_KEYDOWN:
//         break;
//     case WM_KEYUP:
//         break;
//     default:
//         return DefWindowProc(hWnd, message, wParam, lParam);
//     }
//     return 0;
// }