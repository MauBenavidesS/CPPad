#include <windows.h>
#include "resource.h"
#include <tchar.h>
#include "globals.h"

HINSTANCE hInst = GetModuleHandle(NULL);
HWND hEdit = NULL;

void OpenFile(HWND hWnd);
void SaveFile(HWND hWnd);

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK FindReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) 
	{

	case WM_CREATE: 
	{
		// Create the edit control
		hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
			0, 0, 500, 300, hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

		// Load the custom menu from the resource file
		HMENU hMenu = LoadMenu(GetModuleHandle(NULL), MAKEINTRESOURCE(IDC_MAIN));
		SetMenu(hWnd, hMenu);

		break;
	}
	case WM_COMMAND: 
	{
		switch (LOWORD(wParam)) 
		{
		case IDM_OPEN:
			OpenFile(hWnd);
			break;
		case IDM_SAVE:
			SaveFile(hWnd);
			break;
		case IDD_FIND_DIALOG:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_FIND_DIALOG), NULL, FindReplaceDialogProc);
			break;
		}
		break;

	}
	case WM_SIZE: 
	{
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		break;
	}
	case WM_DESTROY: 
	{
		PostQuitMessage(0);
		break;
	}
	default: 
	{
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Register the window class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = TEXT("SimpleEditorClass");
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		MessageBox(NULL, TEXT("Call to RegisterClassEx failed!"), TEXT("Win32 Guided Tour"), 0);
		return 1;
	}

	// Create the window
	HWND hWnd = CreateWindow(TEXT("SimpleEditorClass"), TEXT("Simple Text Editor"),
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		640, 480, NULL, NULL, hInstance, NULL);

	if (!hWnd) {
		MessageBox(NULL, TEXT("Call to CreateWindow failed!"), TEXT("Win32 Guided Tour"), 0);
		return 1;
	}

	// Show the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}