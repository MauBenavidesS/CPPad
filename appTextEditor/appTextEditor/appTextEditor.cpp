#include <windows.h>
#include <tchar.h>
#include "resource.h"

////AWK something to search...
const TCHAR testdata[] =
_T("A \"Hello world\" program is a computer program that outputs \"Hello, world\" on a display")
_T(" device. Because it is typically one of the simplest programs possible in most programming")
_T(" languages, it is by tradition often used to illustrate to beginners the most basic syntax")
_T(" of a programming language, or to verify that a language or system is operating correctly.\r\n")
_T("")
_T("In a device that does not display text, a simple program to produce a signal, such as")
_T(" turning on an LED, is often substituted for \"Hello world\" as the introductory program.");
//// Text from:
//// Hello world program
//// http://en.wikipedia.org/wiki/Hello_world_program

HWND hEdit;
TCHAR g_szFileName[MAX_PATH];

HINSTANCE hInst = NULL;

//// AWT init uFindReplaceMsg at global scope
////UINT uFindReplaceMsg;  // message identifier for FINDMSGSTRING 
const UINT uFindReplaceMsg = RegisterWindowMessage(FINDMSGSTRING);//Regestering FINDMSGSTRING
HWND hdlg = NULL;     // handle to Find dialog box

int findFrom = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//// AWK Pretend file search; searches text which is assumed to be
//// in step with the read-only edit control
int SearchFile(const TCHAR* find, BOOL down, BOOL ignoreCase)
{
	if (0 < _tcslen(find))
	{
		const TCHAR* start = &testdata[findFrom];
		const TCHAR* pos = _tcsstr(start, find);

		if (NULL != pos)
		{
			int foundAt = (pos - testdata);
			findFrom = foundAt + _tcslen(find);
			return foundAt;
		}
	}

	findFrom = 0;
	return -1;
}


// Function to open a file
void OpenFile(HWND hWnd) {
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn)) {
		lstrcpy(g_szFileName, ofn.lpstrFile);
		// Read and display the content of the file
		HANDLE hFile = CreateFile(g_szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwFileSize = GetFileSize(hFile, NULL);
			if (dwFileSize > 0) {
				LPSTR lpContent = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
				if (lpContent != NULL) {
					DWORD dwRead;
					if (ReadFile(hFile, lpContent, dwFileSize, &dwRead, NULL)) {
						SetWindowTextA(hEdit, lpContent);
					}
					GlobalFree(lpContent);
				}
			}
			CloseHandle(hFile);
		}
	}
}

// Function to save a file
void SaveFile(HWND hwnd) {
	OPENFILENAME ofn;
	TCHAR szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = TEXT("Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn)) {
		lstrcpy(g_szFileName, ofn.lpstrFile);
		// Save the content to the file
		HANDLE hFile = CreateFile(g_szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			DWORD dwTextLength = GetWindowTextLength(hEdit);
			LPSTR lpContent = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
			if (lpContent != NULL) {
				GetWindowTextA(hEdit, lpContent, dwTextLength + 1);
				DWORD dwWritten;
				WriteFile(hFile, lpContent, dwTextLength, &dwWritten, NULL);
				GlobalFree(lpContent);
			}
			CloseHandle(hFile);
		}
	}
}

void FindDialog(HWND hwnd) {
	//// Need to be static as their addresses are being used later
	static FINDREPLACE fr;       // common dialog box structure
	// Owner window is passed to us
	static WCHAR szFindWhat[80];  // buffer receiving string

	// Initialize FINDREPLACE
	ZeroMemory(&fr, sizeof(fr));
	fr.lStructSize = sizeof(fr);
	fr.hwndOwner = hwnd;
	fr.lpstrFindWhat = szFindWhat;
	fr.wFindWhatLen = 80;
	fr.Flags = FR_ENABLETEMPLATE | FR_DIALOGTERM;
	//fr.Flags = 0;
	fr.lpTemplateName = MAKEINTRESOURCE(IDD_FIND_DIALOG); // Set your dialog template resource ID
	//fr.lpTemplateName = 0;

	hdlg = FindText(&fr);
}

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
	//// uFindReplaceMsg = RegisterWindowMessage(FINDMSGSTRING);//Regestering FINDMSGSTRING


	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg); //Getting access violation after creating Find dilog box & green arrow showing here.
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit = NULL;
	static HBRUSH hbrWhite = (HBRUSH)GetStockObject(WHITE_BRUSH);

	switch (message)
	{
	case WM_CREATE:
		hEdit = CreateWindow(_T("EDIT"), testdata,
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_NOHIDESEL,
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
			FindDialog(hwnd);
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
		LPFINDREPLACE lpfr;

		if (message == uFindReplaceMsg)
		{
			// Get pointer to FINDREPLACE structure from lParam.
			lpfr = (LPFINDREPLACE)lParam;

			// If the FR_DIALOGTERM flag is set, 
			// invalidate the handle that identifies the dialog box. 
			if (lpfr->Flags & FR_DIALOGTERM)
			{
				hdlg = NULL;
				return 0;
			}

			// If the FR_FINDNEXT flag is set, 
			// call the application-defined search routine
			// to search for the requested string. 
			if (lpfr->Flags & FR_FINDNEXT)
			{
				////For illustrative purposes, just highlight words in Edit control
				int find = SearchFile(lpfr->lpstrFindWhat,
					(BOOL)(lpfr->Flags & FR_DOWN),
					(BOOL)(lpfr->Flags & FR_MATCHCASE));
				if (-1 == find)
				{
					PostMessage(hEdit, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
				}
				else
				{
					int to = (find + _tcslen(lpfr->lpstrFindWhat));
					PostMessage(hEdit, EM_SETSEL, (WPARAM)find, (LPARAM)to);
				}
			}

			return 0;
		}
		else
			return DefWindowProc(hwnd, message, wParam, lParam);

	}
	return 0;
}
