#include <windows.h>


void AdjustFontSize(HWND& hEdit, int& g_nZoomFactor, int change);
INT_PTR  CALLBACK AdjustFontSizeProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
void MouseWheelZoom(HWND hWnd, WPARAM wParam);