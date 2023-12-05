#include <windows.h>
#include "resource.h"

// Definition of the dialog procedure
INT_PTR  CALLBACK FindReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		// Initialization code
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_DIRECTION_UP:
		case IDC_DIRECTION_DOWN:
			// Handle radio button clicks
			// Update your logic based on the selected radio button
			if (IsDlgButtonChecked(hwndDlg, IDC_DIRECTION_UP)) {
				// Up is selected
				// Your logic here
			}
			else {
				// Down is selected
				// Your logic here
			}
			return 0;

		case IDC_REPLACE:
			DestroyWindow(hwndDlg);
			return 0;

		case IDCANCEL:
			return 0;


			// Add cases for other controls as needed

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