// Browser.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "winhttp.h"
#include "Browser.h"
#include <iostream>
#include <vector>
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND button;
const int BTN_CLICKED = 1;
static COLORREF bkcolor = RGB(173, 173, 173);

// Networking primitives
HINTERNET hSession = NULL, hConnect = NULL, hRequest = NULL;
LPCWSTR server = L"www.google.com";
LPCWSTR ext = L"/";
LPCWSTR data = L"/MT4i TradeCopy Sender MT4iTradeCopy 2013-10-11";
BOOL bResults;

// Forward declarations of functions included in this code module:
ATOM                RegisterWindow(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, // this was used in 16 bit windows, but no more
                      _In_ LPWSTR    lpCmdLine, 
                      _In_ int       nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    // TODO: Place code here.
    
    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_BROWSER, szWindowClass, MAX_LOADSTRING);
    RegisterWindow(hInstance);
    
    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow)) {
        return FALSE;
    }
    
    // Accelerators are another name for keyboard shortcuts
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_BROWSER));
    
    MSG msg;
    
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

ATOM RegisterWindow(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BROWSER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_BROWSER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;
    HWND windowHandler = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                       CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
    if (!windowHandler)
        return FALSE;
    ShowWindow(windowHandler, nCmdShow);
    UpdateWindow(windowHandler);
    return TRUE;
}   

LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            button = CreateWindow(TEXT("BUTTON"), TEXT("Go"), WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                  520, 10, 50, 20, windowHandle, (HMENU)BTN_CLICKED, hInst, NULL);
            CreateWindow(TEXT("EDIT"), TEXT(""), WS_BORDER | WS_VISIBLE | WS_CHILD,
                         10, 10, 500, 20, windowHandle, NULL, hInst, NULL);
            break;
        }
        case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
                case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), windowHandle, About);
                break;
                case IDM_EXIT:
                DestroyWindow(windowHandle);
                break;
                case BTN_CLICKED: {
                    DWORD dwSize;
                    LPSTR pszOutBuffer;
                    DWORD dwDownloaded;
                    size_t bodySize;
                    std::vector<LPSTR> responseBody;

                     hSession = WinHttpOpen(  L"A WinHTTP Example Program/1.0", 
                                           WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                           WINHTTP_NO_PROXY_NAME, 
                                           WINHTTP_NO_PROXY_BYPASS, 0);
                    
                    // Specify an HTTP server.
                    if (hSession)
                        hConnect = WinHttpConnect( hSession, server,
                                                  INTERNET_DEFAULT_HTTP_PORT, 0);
                    
                    // Create an HTTP Request handle.
                    if (hConnect)
                        hRequest = WinHttpOpenRequest( hConnect, L"GET", 
                                                      0, 
                                                      ext, WINHTTP_NO_REFERER, 
                                                      WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                      0);
                    
                    // Send a Request.
                    if (hRequest) 
                        bResults = WinHttpSendRequest( hRequest, 
                                                      WINHTTP_NO_ADDITIONAL_HEADERS,
                                                      0, (LPVOID)data, wcslen(data), 
                                                      wcslen(data), 0);
                    if (bResults)
                    {
                        int bodySize = 0;
                        do
                        {
                            // Check for available data.
                            dwSize = 0;
                            if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
                                printf("Error %u in WinHttpQueryDataAvailable.\n", GetLastError());

                            // Allocate space for the buffer.
                            pszOutBuffer = new char[dwSize + 1];
                            if (!pszOutBuffer)
                            {
                                printf("Out of memory\n");
                                dwSize = 0;
                            }
                            else
                            {
                                // Read the data.
                                ZeroMemory(pszOutBuffer, dwSize + 1);

                                if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                                    printf("Error %u in WinHttpReadData.\n", GetLastError());
                                else {
                                    //printf( "%s", pszOutBuffer );
                                    responseBody.push_back(pszOutBuffer);
                                }
                                // Free the memory allocated to the buffer.
                                delete[] pszOutBuffer;
                            }
                        } while (dwSize > 0);
                    }

                    if (hRequest) WinHttpCloseHandle(hRequest);
                    if (hConnect) WinHttpCloseHandle(hConnect);
                    if (hSession) WinHttpCloseHandle(hSession);
                    break;
                }
                default:
                return DefWindowProc(windowHandle, message, wParam, lParam);
            }
        }
        break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(windowHandle, &ps);
            RECT rc;
            GetClientRect(windowHandle, &rc);
            SetDCBrushColor(hdc, bkcolor);
            FillRect(hdc, &rc, (HBRUSH)GetStockObject(DC_BRUSH));
            EndPaint(windowHandle, &ps);
        }
        break;
        case WM_DESTROY: PostQuitMessage(0); break;
        default: return DefWindowProc(windowHandle, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
        case WM_INITDIALOG: return (INT_PTR) TRUE;
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
