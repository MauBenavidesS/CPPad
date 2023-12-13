#include <windows.h>
#include "zoomFunctionality.h"
#include <iostream>
#include <string>
#include "Resource.h"
#include "globals.h"

void MouseWheelZoom(HWND hEdit, WPARAM wParam)
{
	int zDelta = 0;
	zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	// Check if the Ctrl key is pressed
	if (GetKeyState(VK_CONTROL) & 0x8000 && (zDelta != 0)) {
		// Ctrl key is pressed, adjust font size based on mouse wheel direction
		if (zDelta == 120) {
			// Mouse wheel scrolled up
			AdjustFontSize(hEdit, g_nZoomFactor, ZOOMIN_FACTOR);
		}
		else if (zDelta == -120) {
			// Mouse wheel scrolled down
			AdjustFontSize(hEdit, g_nZoomFactor, ZOOMOUT_FACTOR);
		}
	}

}

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

