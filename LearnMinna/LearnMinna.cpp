// LearnMinna.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "LearnMinna.h"
#include "DB.h"

#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")

#define MAX_LOADSTRING 100
#define ID_TABCTRL 1
#define ID_EDIT 2
#define IDS_SUNDAY 0
#define BTN_SEARCH_KANJI 201

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

DB db;
HWND hTab, hDisplay, ghEditSearch, ghBtnSearch, ghStaticKanji;
HWND DocreateTabControl(HWND hwndParent);
HWND DoCreateDisplayWindow(HWND hwndTab);
BOOL OnNotify(HWND hwndTab, HWND hwndDisplay, LPARAM lParam);
BOOL OnNotify2(HWND, LPARAM);
void SearchKanji();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    db.ConnectDB();
    
    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LEARNMINNA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LEARNMINNA));

    MSG msg;

    // Main message loop:
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



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LEARNMINNA));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LEARNMINNA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCITEMW tie = { 0 };
    LRESULT count, id;
    INITCOMMONCONTROLSEX icex;
    RECT rcClient;

    switch (message)
    {
    case WM_CREATE:
    {
        HFONT hFont;
        hFont = CreateFontW(18, 0, 0, 0, FW_THIN, 0, 0, 0, 0, 0, 0, 0, 0, L"Meiryo");

        icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
        icex.dwICC = ICC_TAB_CLASSES;
        InitCommonControlsEx(&icex);

        GetClientRect(hWnd, &rcClient);

        hTab = CreateWindowW(WC_TABCONTROLW, NULL, WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
            0, 0, rcClient.right, rcClient.bottom, hWnd, (HMENU)ID_TABCTRL, hInst, NULL);

        // Kotoba tab
        wchar_t kotobaText[] = L"Kotoba (言葉)";
        tie.mask = TCIF_TEXT;
        tie.pszText = kotobaText;
        count = SendMessageW(hTab, TCM_GETITEMCOUNT, 0, 0);
        SendMessageW(hTab, TCM_INSERTITEMW, count, (LPARAM)(LPTCITEM)&tie);

        // Kanji tab
        wchar_t kanjiText[] = L"Kanji (漢字)";
        tie.mask = TCIF_TEXT;
        tie.pszText = kanjiText;
        count = SendMessageW(hTab, TCM_GETITEMCOUNT, 0, 0);
        SendMessageW(hTab, TCM_INSERTITEMW, count, (LPARAM)(LPTCITEM)&tie);

        // Set tabs font
        SendMessageW(hTab, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Search Kotoba

        // Search Kanji
        ghEditSearch = CreateWindowW(L"Edit", NULL, WS_CHILD | WS_BORDER, 11, 40, 80, 23, hTab, NULL, NULL, NULL);
        SendMessageW(ghEditSearch, WM_SETFONT, (WPARAM)hFont, TRUE);
        ghBtnSearch = CreateWindowW(L"Button", L"Search", WS_CHILD, 94, 40, 100, 23, hWnd, (HMENU)BTN_SEARCH_KANJI, NULL, NULL);
        SendMessageW(ghBtnSearch, WM_SETFONT, (WPARAM)hFont, TRUE);
        ghStaticKanji = CreateWindow(L"Static", L"", WS_CHILD | WS_BORDER | ES_LEFT, 11, 70, 200, 100, hTab, NULL, NULL, NULL, NULL);
        SendMessageW(ghStaticKanji, WM_SETFONT, (WPARAM)hFont, TRUE);


        //hTab = DocreateTabControl(hWnd);
        //hDisplay = DoCreateDisplayWindow(hTab);
    }
    break;
    case WM_NOTIFY:
    {
        //OnNotify(hTab, hDisplay, lParam);
        OnNotify2(hTab, lParam);
    }
    break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case BTN_SEARCH_KANJI:
                SearchKanji();
                break;
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
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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

HWND DocreateTabControl(HWND hwndParent)
{
    RECT rcClient;
    INITCOMMONCONTROLSEX icex;
    HWND hwndTab;
    TCITEM tie;
    int i;
    TCHAR achTemp[256];  // Temporary buffer for strings.

    // Initialize common controls.
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

    // Get the dimensions of the parent window's client area, and 
    // create a tab control child window of that size. Note that g_hInst
    // is the global instance handle.
    GetClientRect(hwndParent, &rcClient);
    hwndTab = CreateWindow(WC_TABCONTROL, L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
        0, 0, rcClient.right, rcClient.bottom,
        hwndParent, NULL, hInst, NULL);
    if (hwndTab == NULL)
    {
        return NULL;
    }

    // Add tabs for each day of the week. 
    tie.mask = TCIF_TEXT | TCIF_IMAGE;
    tie.iImage = -1;
    tie.pszText = achTemp;

    for (i = 0; i < 7; i++)
    {
        // Load the day string from the string resources. Note that
        // g_hInst is the global instance handle.
        LoadString(hInst, IDS_SUNDAY + i,
            achTemp, sizeof(achTemp) / sizeof(achTemp[0]));
        
        swprintf_s(achTemp, L"Day %d", i);
        tie.pszText = achTemp;

        if (TabCtrl_InsertItem(hwndTab, i, &tie) == -1)
        {
            DestroyWindow(hwndTab);
            return NULL;
        }
    }
    return hwndTab;
}

HWND DoCreateDisplayWindow(HWND hwndTab)
{
    HWND hwndStatic = CreateWindowW(WC_STATIC, L"",
        WS_CHILD | WS_VISIBLE | WS_BORDER,
        100, 100, 100, 100,        // Position and dimensions; example only.
        hwndTab, NULL, hInst,    // g_hInst is the global instance handle
        NULL);
    return hwndStatic;
}

BOOL OnNotify(HWND hwndTab, HWND hwndDisplay, LPARAM lParam)
{
    TCHAR achTemp[256]; // temporary buffer for strings

    switch (((LPNMHDR)lParam)->code)
    {
    case TCN_SELCHANGING:
    {
        // Return FALSE to allow the selection to change.
        return FALSE;
    }

    case TCN_SELCHANGE:
    {
        int iPage = TabCtrl_GetCurSel(hwndTab);

        // Note that g_hInst is the global instance handle.
        LoadString(hInst, IDS_SUNDAY + iPage, achTemp,
            sizeof(achTemp) / sizeof(achTemp[0]));

        swprintf_s(achTemp, L"%d", iPage);

        //MessageBoxW(hwndTab, achTemp, achTemp, MB_OK);

        LRESULT result = SendMessageW(hwndDisplay, WM_SETTEXT, 0,
            (LPARAM)achTemp);
        if (iPage == 3)
        {
            ShowWindow(hwndDisplay, SW_HIDE);
            
        }        
        else
        {
            ShowWindow(hwndDisplay, SW_SHOW);
        }
        break;
    }
    }
    return TRUE;
}

BOOL OnNotify2(HWND hwndTab, LPARAM lParam)
{
    TCHAR achTemp[256]; // temporary buffer for strings

    switch (((LPNMHDR)lParam)->code)
    {
    case TCN_SELCHANGING:
    {
        // Return FALSE to allow the selection to change.
        return FALSE;
    }

    case TCN_SELCHANGE:
    {
        int iPage = TabCtrl_GetCurSel(hwndTab);
        if (iPage == 0) 
        {
            ShowWindow(ghEditSearch, SW_HIDE);
            SetWindowTextW(ghStaticKanji, L"");
            ShowWindow(ghBtnSearch, SW_HIDE);
            ShowWindow(ghStaticKanji, SW_HIDE);
        }
        else
        {
            ShowWindow(ghEditSearch, SW_SHOW);           
            ShowWindow(ghBtnSearch, SW_SHOW);
            ShowWindow(ghStaticKanji, SW_SHOW);
        }
        break;
    }
    }
    return TRUE;
}

void SearchKanji()
{
    wchar_t search[5];
    search[0] = '\0';
    GetWindowTextW(ghEditSearch, search, 5);

    if (search[0] == '\0' || search == NULL)
    {
        //MessageBox(hTab, L"Please enter kanji to search", L"Search Kanji", MB_OK);
        SetWindowTextW(ghStaticKanji, L"Please enter kanji to search");
        return;
    }

    HSTMT hstmt = db.search_kanji(search);
    int rowCount = 0;
    while (SQLFetch(hstmt) == SQL_SUCCESS)
    {
        rowCount++;

        WCHAR wrmj[5], whrgn[50], wktkn[50], wmsg[150];

        SQLGetData(hstmt, 2, SQL_C_WCHAR, wrmj, 5, NULL);
        SQLGetData(hstmt, 3, SQL_C_WCHAR, whrgn, 50, NULL);
        SQLGetData(hstmt, 4, SQL_C_WCHAR, wktkn, 150, NULL);

        wsprintf(wmsg, L"%ws %ws %ws", wrmj, whrgn, wktkn);
        SetWindowTextW(ghStaticKanji, wmsg);
    }

    if (rowCount == 0)
        SetWindowTextW(ghStaticKanji, L"Kanji not found");
       
}
