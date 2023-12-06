#include <windows.h>
#include "resource.h"
#include "globals.h"

#include <atlbase.h>
#include <atlapp.h> // must be included

TCHAR szFindWhat[256];
TCHAR szReplaceWith[256];

bool FindFirst(HWND hwndDlg)
{ // Returns TRUE if text to search for was found, else it returns FALSE.
	// Get the search string from the edit control in the Find Dialog Box
	GetDlgItemText(hwndDlg, IDC_FIND_WHAT, szFindWhat, sizeof(szFindWhat) / sizeof(szFindWhat[0]));

	// Get the text from the main text editor control
	int textLength = GetWindowTextLength(hEdit);
	TCHAR* buffer = new TCHAR[textLength + 1];
	GetWindowText(hEdit, buffer, textLength + 1);

	// Perform the search operation
	TCHAR* position = _tcsstr(buffer, szFindWhat);
	if (position != NULL) {
		// Found the search string
		int startPosition = position - buffer;
		int endPosition = startPosition + _tcslen(szFindWhat);

		// Select the found text in the main text editor control
		SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);
		return TRUE;
	}
	else 
	{
		return FALSE;
	}

	delete[] buffer;
}
void ReplaceTextWith(HWND hwndDlg)
{ // Replaces the text in the first EDITTEXT input bar with the text in the second EDITTEXT input bar. 
	// Get the search string from the edit control in the Find Dialog Box
	GetDlgItemText(hwndDlg, IDC_FIND_WHAT, szFindWhat, sizeof(szFindWhat) / sizeof(szFindWhat[0]));
	GetDlgItemText(hwndDlg, IDC_REPLACE_WITH, szReplaceWith, sizeof(szReplaceWith) / sizeof(szReplaceWith[0]));

	// Get the text from the main text editor control
	int textLength = GetWindowTextLength(hEdit);
	TCHAR* buffer = new TCHAR[textLength + 1];
	GetWindowText(hEdit, buffer, textLength + 1);

	// Perform the search operation
	TCHAR* position = _tcsstr(buffer, szFindWhat);
	if (position != NULL) {
		// Found the search string
		int startPosition = position - buffer;
		int endPosition = startPosition + _tcslen(szFindWhat);

		// Select the found text in the main text editor control
		SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);

		// Output text to the main text editor control
		SendMessage(hEdit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(szReplaceWith));
	}
	else {
		// Not found
		MessageBox(hwndDlg, TEXT("Text not found!"), TEXT("Find Next"), MB_OK | MB_ICONINFORMATION);
	}

	delete[] buffer;
}

// Definition of the dialog procedure
INT_PTR  CALLBACK FindReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		// Initialization code
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_DIRECTION_UP:
			// Handle radio button clicks
			if (IsDlgButtonChecked(hwndDlg, IDC_DIRECTION_DOWN)) {
				SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_UP), BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_DOWN), BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else {
				SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_UP), BM_SETCHECK, BST_CHECKED, 0);
			}
			return 0;

		case IDC_DIRECTION_DOWN:
			// Handle radio button clicks
			if (IsDlgButtonChecked(hwndDlg, IDC_DIRECTION_UP)) {
				SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_DOWN), BM_SETCHECK, BST_CHECKED, 0);
				SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_UP), BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else {
				SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_DOWN), BM_SETCHECK, BST_CHECKED, 0);
			}
			return 0;

		case IDC_FIND_NEXT:
		{
			bool foundText = FindFirst(hwndDlg);
			if (foundText == FALSE)
			{
				MessageBox(hwndDlg, TEXT("Text not found."), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
			}
			return 0;
		}

		case IDC_REPLACE:
		{
			bool somethingToReplace = FindFirst(hwndDlg);
			if (somethingToReplace == TRUE) 
			{
				ReplaceTextWith(hwndDlg);
				FindFirst(hwndDlg);
			}
			else 
			{
				MessageBox(hwndDlg, TEXT("No text to replace."), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
			}
			return 0;
		}


		case IDC_REPLACE_ALL:
			return 0;

		case IDCANCEL:
			DestroyWindow(hwndDlg);
			return 0;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		// Handle close message
		EndDialog(hwndDlg, IDCANCEL);
		return TRUE;

	default:
		return FALSE; // Let Windows handle other messages
	}

	return FALSE;
}