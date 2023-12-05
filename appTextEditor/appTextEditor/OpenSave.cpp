#include <windows.h>
#include "resource.h"
#include <tchar.h>

#include "globals.h"

TCHAR g_szFileName[MAX_PATH];

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
