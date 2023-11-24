#include <iostream>
#include <Windows.h>
#include <commdlg.h>
#include <Richedit.h>

#define MAX_FIND_REPLACE_LENGTH 256 // Adjust the length as needed

// Menu command IDs
#define ID_FILE_OPEN     1001
#define ID_FILE_SAVE     1002
#define ID_EDIT_FIND     1003
#define ID_EDIT_REPLACE  1004
#define IDD_REPLACE_DIALOG  1005 // Use the appropriate value

// Edit control ID
#define IDOK    1
#define IDCANCEL    2
#define IDC_EDIT_TEXT    3

class SimpleEditor {
public:
    SimpleEditor(HINSTANCE hInstance) : hInstance(hInstance), hWnd(nullptr), hEdit(nullptr), g_szFileName() {
        Initialize();
    }

    int Run(int nCmdShow) {
        // Create the main window
        if (!CreateMainWindow()) {
            return 1;
        }

        // Show and update the window
        ShowWindow(hWnd, nCmdShow);
        UpdateWindow(hWnd);

        // Message loop
        MSG msg;
        while (GetMessage(&msg, nullptr, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        return static_cast<int>(msg.wParam);
    }

private:
    HINSTANCE hInstance;
    HWND hWnd;
    HWND hEdit;
    TCHAR g_szFileName[MAX_PATH];

    void Initialize() {
        hInstance = nullptr;
        hWnd = nullptr;
        hEdit = nullptr;
        g_szFileName[0] = '\0';
    }

    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        SimpleEditor* pEditor;

        if (message == WM_CREATE) {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            pEditor = reinterpret_cast<SimpleEditor*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pEditor));
        }
        else {
            pEditor = reinterpret_cast<SimpleEditor*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }

        if (pEditor) {
            return pEditor->HandleMessage(hWnd, message, wParam, lParam);
        }

        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    bool CreateMainWindow() {
        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = L"SimpleEditorClass";
        wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

        if (!RegisterClassEx(&wcex)) {
            MessageBox(nullptr, L"Call to RegisterClassEx failed!", L"Win32 Guided Tour", 0);
            return false;
        }

        hWnd = CreateWindowEx(
            0,
            L"SimpleEditorClass",
            L"Simple Text Editor",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            640,
            480,
            nullptr,
            nullptr,
            hInstance,
            this
        );

        if (!hWnd) {
            MessageBox(nullptr, L"Call to CreateWindow failed!", L"Win32 Guided Tour", 0);
            return false;
        }

        hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
            0, 0, 500, 300, hWnd, nullptr, hInstance, nullptr);

        if (!hEdit) {
            MessageBox(nullptr, L"Call to CreateWindowEx for Edit control failed!", L"Win32 Guided Tour", 0);
            return false;
        }

        // Create the menu bar
        HMENU hMenu = CreateMenu();
        HMENU hFileMenu = CreateMenu();

        // Add "Open" and "Save" options to the "File" menu
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_OPEN, L"Open");
        AppendMenu(hFileMenu, MF_STRING, ID_FILE_SAVE, L"Save");

        // Add "Find" and "Replace" options to the "Edit" menu
        HMENU hEditMenu = CreateMenu();
        AppendMenu(hEditMenu, MF_STRING, ID_EDIT_FIND, L"Find");
        AppendMenu(hEditMenu, MF_STRING, ID_EDIT_REPLACE, L"Replace");

        // Add the "File" menu to the menu bar
        AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hFileMenu), L"File");
        AppendMenu(hMenu, MF_POPUP, reinterpret_cast<UINT_PTR>(hEditMenu), L"Edit");

        // Set the menu bar for the window
        SetMenu(hWnd, hMenu);

        return true;
    }

    LRESULT HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
            case ID_FILE_OPEN:
                OpenFile();
                break;

            case ID_FILE_SAVE:
                SaveFile();
                break;

            case ID_EDIT_FIND:
                ShowFindDialog();
                break;

            case ID_EDIT_REPLACE:
                ShowReplaceDialog(hWnd);
                break;
            }
            break;
        }

        case WM_SIZE: {
            MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
            break;
        }

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        return 0;
    }

    void OpenFile() {
        OPENFILENAME ofn;
        TCHAR szFile[MAX_PATH];

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWnd;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
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

    void SaveFile() {
        OPENFILENAME ofn;
        TCHAR szFile[MAX_PATH];

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = hWnd;
        ofn.lpstrFile = szFile;
        ofn.lpstrFile[0] = '\0';
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrFileTitle = nullptr;
        ofn.nMaxFileTitle = 0;
        ofn.lpstrInitialDir = nullptr;
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

    void ShowFindDialog() {
        FINDREPLACE findReplace;
        CHAR szFind[MAX_FIND_REPLACE_LENGTH];
        WCHAR wszFind[MAX_FIND_REPLACE_LENGTH]; // Wide character buffer

        ZeroMemory(&findReplace, sizeof(FINDREPLACE));
        findReplace.lStructSize = sizeof(FINDREPLACE);
        findReplace.hwndOwner = hWnd;

        // Assume that szFind contains the search string in ANSI encoding
        // Convert ANSI to Unicode
        MultiByteToWideChar(CP_ACP, 0, szFind, -1, wszFind, MAX_FIND_REPLACE_LENGTH);

        // Use the wide character buffer for lpstrFindWhat
        findReplace.lpstrFindWhat = wszFind;

        findReplace.Flags = FR_DOWN | FR_MATCHCASE;

        if (FindText(&findReplace) == NULL) {
            DWORD dwError = CommDlgExtendedError();
            if (dwError != 0) {
                std::cerr << "Error in FindText: " << dwError << std::endl;
            }
        }
    }

    void ShowReplaceDialog(HWND hwndDlg) {
        FINDREPLACE findReplace;
        CHAR szFind[MAX_FIND_REPLACE_LENGTH];
        CHAR szReplace[MAX_FIND_REPLACE_LENGTH];
        WCHAR wszFind[MAX_FIND_REPLACE_LENGTH];
        WCHAR wszReplace[MAX_FIND_REPLACE_LENGTH];
        ZeroMemory(&findReplace, sizeof(FINDREPLACE));

        findReplace.lStructSize = sizeof(FINDREPLACE);
        findReplace.hwndOwner = hwndDlg;
        findReplace.lpstrFindWhat = wszFind;
        findReplace.lpstrReplaceWith = wszReplace;

        // Convert ANSI to Unicode
        MultiByteToWideChar(CP_ACP, 0, szFind, -1, wszFind, MAX_FIND_REPLACE_LENGTH);
        MultiByteToWideChar(CP_ACP, 0, szReplace, -1, wszReplace, MAX_FIND_REPLACE_LENGTH);

        // Allocate memory for the Unicode strings
        findReplace.lpstrFindWhat = new WCHAR[MAX_FIND_REPLACE_LENGTH];
        findReplace.lpstrReplaceWith = new WCHAR[MAX_FIND_REPLACE_LENGTH];

        // Copy content into the allocated buffers
        wcscpy_s(findReplace.lpstrFindWhat, MAX_FIND_REPLACE_LENGTH, wszFind);
        wcscpy_s(findReplace.lpstrReplaceWith, MAX_FIND_REPLACE_LENGTH, wszReplace);

        findReplace.Flags = FR_DOWN | FR_MATCHCASE;

        if (ReplaceText(&findReplace) == NULL) {
            DWORD dwError = CommDlgExtendedError();
            if (dwError != 0) {
                std::cerr << "Error in ReplaceText: " << dwError << std::endl;
            }
        }

        // Don't forget to free the allocated memory
        delete[] findReplace.lpstrFindWhat;
        delete[] findReplace.lpstrReplaceWith;
    }

    void HandleOK(HWND hwndDlg) {
        // User clicked OK, continue with the replace operation
        // ...
        EndDialog(hwndDlg, IDOK);
    }

    void HandleCancel(HWND hwndDlg) {
        // User clicked Cancel, handle as needed
        // ...
        EndDialog(hwndDlg, IDCANCEL);
    }

    void HandleEditText(HWND hwndDlg) {
        // Handle events related to the edit control
        // For example, you can use GetDlgItemText to get the text from the edit control.
        // ...

        // Notify the dialog that you've handled the event
        EndDialog(hwndDlg, IDC_EDIT_TEXT);
    }

    static INT_PTR CALLBACK ReplaceDialogProcStatic(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
        SimpleEditor* pEditor;

        if (message == WM_INITDIALOG) {
            SetWindowLongPtr(hwndDlg, GWLP_USERDATA, static_cast<LONG_PTR>(lParam));
        }

        pEditor = reinterpret_cast<SimpleEditor*>(GetWindowLongPtr(hwndDlg, GWLP_USERDATA));

        if (pEditor) {
            return pEditor->ReplaceDialogProc(hwndDlg, message, wParam, lParam);
        }

        return FALSE;
    }

    INT_PTR CALLBACK ReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
        case WM_INITDIALOG:
            // Initialization logic here
            return TRUE;

        case WM_COMMAND:
            switch (LOWORD(wParam)) {
            case IDOK:
                HandleOK(hwndDlg);
                return TRUE;

            case IDCANCEL:
                HandleCancel(hwndDlg);
                return TRUE;

            case IDC_EDIT_TEXT:
                HandleEditText(hwndDlg);
                return TRUE;
            }
            break;

            // Handle additional messages if needed

        default:
            return FALSE;
        }

        return FALSE;
    }

};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SimpleEditor editor(hInstance);
    return editor.Run(nCmdShow);
}