#pragma once

void AdjustFontSize(HWND& hEdit, int& g_nZoomFactor, int change);
void MouseWheelZoom(HWND hWnd, WPARAM wParam);

INT_PTR  CALLBACK AdjustFontSizeProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
