#pragma once

bool FindNext(HWND hwndDlg);
void ReplaceTextWith(HWND hwndDlg);
void ReplaceAll(HWND hwndDlg);

INT_PTR CALLBACK FindReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
