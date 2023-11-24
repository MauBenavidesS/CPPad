#include <Windows.h>
#include <commdlg.h>
#include <Richedit.h>

#define MAX_FIND_REPLACE_LENGTH 256 // Adjust the length as needed

// Global variables
HWND g_hWndEdit;
TCHAR g_szFileName[MAX_PATH];
HWND g_hDlgFindReplace = NULL;
FINDREPLACE g_findReplace;

// Variables for find and replace text
TCHAR g_szFindText[MAX_FIND_REPLACE_LENGTH] = TEXT("");
TCHAR g_szReplaceText[MAX_FIND_REPLACE_LENGTH] = TEXT("");

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
                        SetWindowTextA(g_hWndEdit, lpContent);
                    }
                    GlobalFree(lpContent);
                }
            }
            CloseHandle(hFile);
        }
    }
}

// Function to save a file
void SaveFile(HWND hWnd) {
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
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn)) {
        lstrcpy(g_szFileName, ofn.lpstrFile);

        // Save the content to the file
        HANDLE hFile = CreateFile(g_szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            DWORD dwTextLength = GetWindowTextLength(g_hWndEdit);
            LPSTR lpContent = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
            if (lpContent != NULL) {
                GetWindowTextA(g_hWndEdit, lpContent, dwTextLength + 1);
                DWORD dwWritten;
                WriteFile(hFile, lpContent, dwTextLength, &dwWritten, NULL);
                GlobalFree(lpContent);
            }
            CloseHandle(hFile);
        }
    }
}

// Function to perform the Find operation
void FindText(HWND hWnd) {
    CHARRANGE cr;
    SendMessage(g_hWndEdit, EM_EXGETSEL, 0, (LPARAM)&cr);

    g_findReplace.Flags = FR_DOWN | FR_MATCHCASE; // Set your desired flags

    if (g_hDlgFindReplace == NULL) {
        g_hDlgFindReplace = FindText(&g_findReplace);
    }
    else {
        FINDTEXTEX findText;
        findText.chrg = cr;
        findText.lpstrText = g_findReplace.lpstrFindWhat;

        // Perform the find operation
        int pos = SendMessage(g_hWndEdit, EM_FINDTEXTEX, g_findReplace.Flags, (LPARAM)&findText);
        if (pos != -1) {
            // Select the found text
            SendMessage(g_hWndEdit, EM_EXSETSEL, 0, (LPARAM)&findText.chrgText);

            // Optionally scroll the found text into view
            SendMessage(g_hWndEdit, EM_SCROLLCARET, 0, 0);
        }
        else {
            // Inform the user that the text was not found
            MessageBox(hWnd, TEXT("Text not found."), TEXT("Find"), MB_OK | MB_ICONINFORMATION);
        }
    }
}

// Function to perform the Replace operation
void ReplaceText(HWND hWnd) {
    CHARRANGE cr;
    SendMessage(g_hWndEdit, EM_EXGETSEL, 0, (LPARAM)&cr);

    g_findReplace.Flags = FR_DOWN | FR_MATCHCASE; // Set your desired flags

    if (g_hDlgFindReplace == NULL) {
        g_hDlgFindReplace = ReplaceText(&g_findReplace);
    }
    else {
        FINDTEXTEX findText;
        findText.chrg = cr;
        findText.lpstrText = g_findReplace.lpstrFindWhat;

        // Perform the find operation
        int pos = SendMessage(g_hWndEdit, EM_FINDTEXTEX, g_findReplace.Flags, (LPARAM)&findText);
        if (pos != -1) {
            // Select the found text
            SendMessage(g_hWndEdit, EM_EXSETSEL, 0, (LPARAM)&findText.chrgText);

            // Optionally scroll the found text into view
            SendMessage(g_hWndEdit, EM_SCROLLCARET, 0, 0);

            // Replace the selected text
            SendMessage(g_hWndEdit, EM_REPLACESEL, TRUE, (LPARAM)g_findReplace.lpstrReplaceWith);
        }
        else {
            // Inform the user that the text was not found
            MessageBox(hWnd, TEXT("Text not found."), TEXT("Replace"), MB_OK | MB_ICONINFORMATION);
        }
    }
}

// Function to show the Find dialog
void ShowFindDialog(HWND hWnd) {
    g_findReplace.lpstrFindWhat = g_szFindText;
    g_findReplace.Flags = FR_DOWN | FR_MATCHCASE; // Set your desired flags

    g_hDlgFindReplace = FindText(&g_findReplace);
}

// Function to show the Replace dialog
void ShowReplaceDialog(HWND hWnd) {
    g_findReplace.lpstrFindWhat = g_szFindText;
    g_findReplace.lpstrReplaceWith = g_szReplaceText;
    g_findReplace.Flags = FR_DOWN | FR_MATCHCASE; // Set your desired flags

    g_hDlgFindReplace = ReplaceText(&g_findReplace);
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        // Define command IDs
    #define ID_FILE_OPEN  1001
    #define ID_FILE_SAVE  1002
    #define ID_EDIT_FIND  1003
    #define ID_EDIT_REPLACE  1004

    case WM_CREATE: {
        // Create the edit control
        g_hWndEdit = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""),
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 500, 300, hWnd, (HMENU)1, GetModuleHandle(NULL), NULL);

        // Create the menu bar
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreateMenu();

        // Add "Open" and "Save" options to the "File" menu
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, TEXT("Open"));
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, TEXT("Save"));

        // Add "Find" and "Replace" options to the "Edit" menu
        HMENU hEditMenu = CreateMenu();
        AppendMenu(hEditMenu, MF_STRING, ID_EDIT_FIND, TEXT("Find"));
        AppendMenu(hEditMenu, MF_STRING, ID_EDIT_REPLACE, TEXT("Replace"));

        // Add the "File" menu to the menu bar
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, TEXT("File"));
        AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hEditMenu, TEXT("Edit"));

        // Set the menu bar for the window
        SetMenu(hWnd, hMenu);

        break;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_FILE_OPEN:
            OpenFile(hWnd);
            break;
        case ID_FILE_SAVE:
            SaveFile(hWnd);
            break;
        case ID_EDIT_FIND:
            ShowFindDialog(hWnd);
            break;
        case ID_EDIT_REPLACE:
            ShowReplaceDialog(hWnd);
            break;
        }
        break;
    }
    case WM_SIZE: {
        MoveWindow(g_hWndEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default: {
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Register the window class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = TEXT("SimpleEditorClass");
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, TEXT("Call to RegisterClassEx failed!"), TEXT("Win32 Guided Tour"), 0);
        return 1;
    }

    // Create the window
    HWND hWnd = CreateWindow(TEXT("SimpleEditorClass"), TEXT("Simple Text Editor"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480, NULL, NULL, hInstance, NULL);

    if (!hWnd) {
        MessageBox(NULL, TEXT("Call to CreateWindow failed!"), TEXT("Win32 Guided Tour"), 0);
        return 1;
    }

    // Show the window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
