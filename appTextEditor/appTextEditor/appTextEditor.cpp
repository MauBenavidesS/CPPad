#include <windows.h>
#include "resource.h"
#include <tchar.h>
#include "globals.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FindReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

void OpenFile(HWND hWnd);
void SaveFile(HWND hWnd);

HINSTANCE hInst = GetModuleHandle(NULL);
HWND hEdit = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("TextFind Demo"); //// AWK
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass = { 0 }; //// AWK zero struct

	hInst = hInstance; //// AWK remember instance 

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = NULL; //// AWK was (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName = MAKEINTRESOURCE(IDC_MAIN); //// AWK added menu (was NULL)
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Program requires Windows NT!"),
			szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName, TEXT("Find Text in Edit Control Demo"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	//// init now done at global scope


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); //Getting access violation after creating Find dilog box & green arrow showing here.
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hbrWhite = (HBRUSH)GetStockObject(WHITE_BRUSH);

	switch (message)
	{
	case WM_CREATE:
		hEdit = CreateWindow(_T("EDIT"), L"",
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_NOHIDESEL,
			0, 0, 0, 0, hwnd, (HMENU)100, hInst, NULL);
		return 0;

	case WM_SIZE:
		if (NULL != hEdit)
			MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), FALSE);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_OPEN:
			OpenFile(hwnd);
			return 0;
		case IDM_SAVE:
			SaveFile(hwnd);
			return 0;
		case IDD_FIND_DIALOG:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FIND_DIALOG), NULL, FindReplaceDialogProc);
			//FindDialog(hwnd);
			return 0;

		case IDM_EXIT:
			DestroyWindow(hwnd);
			return 0;
		}
		break;

		//// Make read-only Edit control white
	case WM_CTLCOLORSTATIC:
		return (LRESULT)hbrWhite;

	case WM_PAINT://paint operation
		return 0;

		//// Edit control handles its own background, so do not
		//// repaint background (avoids flicker.)
	case WM_ERASEBKGND:
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);

	}
	return 0;
}
