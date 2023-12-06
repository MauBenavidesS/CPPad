#include "zoomFunctionality.h"
#include <iostream>
#include <string>
#include "Resource.h"
#include "globals.h"

void AdjustFontSize(HWND& hEdit, int& g_nZoomFactor, int change) {

	if (g_nZoomFactor+change >0 && g_nZoomFactor + change < 100) {
		g_nZoomFactor += change;
	}

	// Create a new font with the new size
	HFONT hNewFont = CreateFont(g_nZoomFactor, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE);

	// Set the new font in the text editor
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hNewFont, TRUE);

}

// Definition of the dialog procedure
INT_PTR  CALLBACK AdjustFontSizeProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:

		return TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case ID_ZOOM_IN:
			AdjustFontSize(hEdit, g_nZoomFactor, ZOOMIN_FACTOR);
			break;
		case ID_ZOOM_OUT:
			AdjustFontSize(hEdit, g_nZoomFactor, ZOOMOUT_FACTOR);
			break;

		case IDCANCEL:
			EndDialog(hwndDlg, IDCANCEL);
			return TRUE;
		}
	}
	default:
		return FALSE; // Let Windows handle other messages
	}

	return FALSE;
}

