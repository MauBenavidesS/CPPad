#include "zoomFunctionality.h"
#include <iostream>
#include <string>

void AdjustFontSize(HWND& hEdit, int& g_nZoomFactor, int change) {
	
	g_nZoomFactor += change;

	// Create a new font with the new size
	HFONT hNewFont = CreateFont(g_nZoomFactor, 0, 0, 0, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
		FALSE, FALSE, FALSE, FALSE);

	// Set the new font in the text editor
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hNewFont, TRUE);	
	
}
