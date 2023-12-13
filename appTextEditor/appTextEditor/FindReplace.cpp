#include <windows.h>
#include "resource.h"
#include "globals.h"
#include <tchar.h>
#include <vector>
#include <string>

#include <atlbase.h>
#include <atlapp.h> // Must Download WTL and add it to Additional Libraries' include path

TCHAR szFindWhat[256];
TCHAR szReplaceWith[256];

TCHAR* position = nullptr;  // Use static variable to maintain position between calls

enum DIRECTION {
	DOWN,
	UP
};
DIRECTION currentDirection = DOWN;

bool FindNext(HWND hwndDlg)
{
	// Get the search string from the edit control in the Find Dialog Box
	GetDlgItemText(hwndDlg, IDC_FIND_WHAT, szFindWhat, sizeof(szFindWhat) / sizeof(szFindWhat[0]));

	// Get the text from the main text editor control
	int textLength = GetWindowTextLength(hEdit);
	TCHAR* buffer = new TCHAR[textLength + 1];
	GetWindowText(hEdit, buffer, textLength + 1);

	int wordCount = 0;
	std::vector<TCHAR*> pStartPositionsVector;

	int startPosition = textLength + 1;
	int endPosition = textLength + 1;

	if (position == nullptr) {
		position = buffer;  // If position is null, start from the beginning
	}

	TCHAR* foundWordsPosition = buffer;
	while ((foundWordsPosition = _tcsstr(foundWordsPosition, szFindWhat)) != NULL)
	{
		int localStartPosition = foundWordsPosition - buffer;
		int localEndPosition = localStartPosition + _tcslen(szFindWhat);

		// Stores starting positions of each found word
		pStartPositionsVector.push_back(foundWordsPosition);

		// Move the position forward to search for the next occurrence
		foundWordsPosition += _tcslen(szFindWhat);  // Increment the pointer to the end of the found text
	}
	wordCount = pStartPositionsVector.size();

	if (currentDirection == DIRECTION::DOWN)
	{
		if ((position = _tcsstr(position, szFindWhat)) != NULL)
		{
			// Found the search string
			startPosition = position - buffer;
			endPosition = startPosition + _tcslen(szFindWhat);

			// Select the found text in the main text editor control
			SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);

			// Move the position forward to search for the next occurrence
			position += _tcslen(szFindWhat);  // Increment the pointer to the end of the found text
		}
		else
		{
			std::wstring message = L"Text could not be found ahead. \n Text instances found: " + std::to_wstring(wordCount);
			MessageBox(hwndDlg, message.c_str(), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
		}
	}
	else if (currentDirection == DIRECTION::UP)
	{
		if (!pStartPositionsVector.empty())
		{
			std::wstring posStr = std::to_wstring((position) - buffer) + L"\n";

			for (size_t i = pStartPositionsVector.size()-1; i > 0; --i)
			{
				// if pointer is null, the pointer is set to the last word's position.
				if ((position - buffer) == 0) 
				{
					position = pStartPositionsVector[pStartPositionsVector.size()-1];
					startPosition = (position - buffer);

					//startPosition = position - buffer;
					endPosition = startPosition + _tcslen(szFindWhat);

					// Select the found text in the main text editor control
					SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);
					break;
				}

				// if the pointer is in the first word, set the pointer to the last word's position
				if ((position - buffer) == (pStartPositionsVector[0] - buffer)) 
				{
					position = pStartPositionsVector[pStartPositionsVector.size() - 1];
					startPosition = (position - buffer);

					//startPosition = position - buffer;
					endPosition = startPosition + _tcslen(szFindWhat);

					// Select the found text in the main text editor control
					SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);
					break;
				}

				// set the pointer to the previous word
				int currentWordsPosition = (pStartPositionsVector[i - 1] - buffer);
				if ((currentWordsPosition < (position - buffer)))
				{
					position = pStartPositionsVector[i - 1];
					startPosition = currentWordsPosition;

					//startPosition = position - buffer;
					endPosition = startPosition + _tcslen(szFindWhat);

					// Select the found text in the main text editor control
					SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);
					break;
				}
			}

		}
		else
		{
			std::wstring message = L"Text instances found: " + std::to_wstring(wordCount);
			MessageBox(hwndDlg, message.c_str(), TEXT("Information"), MB_OK | MB_ICONINFORMATION);
		}

	}

	delete[] buffer;

	// Scroll the caret into view
	SendMessage(hEdit, EM_SCROLLCARET, 0, 0);
	return true;

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

	if (position == nullptr) {
		position = buffer;  // If position is null, start from the beginning
	}

	// Move the position forward to search for the next occurrence
	position -= _tcslen(szFindWhat);  // Increment the pointer to the end of the found text
	if ((position = _tcsstr(position, szFindWhat)) != NULL)
	{
		// Found the search string
		int startPosition = position - buffer;
		int endPosition = startPosition + _tcslen(szFindWhat);

		// Select the found text in the main text editor control
		SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);

		SendMessage(hEdit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(szReplaceWith));

		SendMessage(hEdit, EM_SETSEL, startPosition, startPosition + _tcslen(szReplaceWith));

	}

	delete[] buffer;
}

void ReplaceAll(HWND hwndDlg)
{
	// Get the search string from the edit control in the Find Dialog Box
	GetDlgItemText(hwndDlg, IDC_FIND_WHAT, szFindWhat, sizeof(szFindWhat) / sizeof(szFindWhat[0]));
	GetDlgItemText(hwndDlg, IDC_REPLACE_WITH, szReplaceWith, sizeof(szReplaceWith) / sizeof(szReplaceWith[0]));

	// Get the text from the main text editor control
	int textLength = GetWindowTextLength(hEdit);
	TCHAR* buffer = new TCHAR[textLength + 1];
	GetWindowText(hEdit, buffer, textLength + 1);

	// Perform the search operation
	int replaceOffset = 0; // Track the offset caused by replacements
	TCHAR* position = buffer;

	int counter = 0;
	while ((position = _tcsstr(position, szFindWhat)) != NULL)
	{
		counter += 1;
		// Found the search string
		int startPosition = position - buffer + replaceOffset;
		int endPosition = startPosition + _tcslen(szFindWhat);

		// Select the found text in the main text editor control
		SendMessage(hEdit, EM_SETSEL, startPosition, endPosition);

		// Output text to the main text editor control
		SendMessage(hEdit, EM_REPLACESEL, TRUE, reinterpret_cast<LPARAM>(szReplaceWith));

		// Calculate the new offset caused by the replacement
		int replacementLength = _tcslen(szReplaceWith);
		int originalLength = _tcslen(szFindWhat);
		replaceOffset += replacementLength - originalLength;

		// Move the position forward to search for the next occurrence
		position += originalLength; // Increment the pointer to the end of the found text
	}

	std::wstring str = std::to_wstring(counter) + TEXT(" Instances of the text were replaced.");
	MessageBox(hwndDlg, str.c_str(), TEXT("Information"), MB_OK | MB_ICONINFORMATION);

	delete[] buffer;
}

// Definition of the dialog procedure
INT_PTR  CALLBACK FindReplaceDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_INITDIALOG:
		// Initialization code
		SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_DOWN), BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(GetDlgItem(hwndDlg, IDC_DIRECTION_UP), BM_SETCHECK, BST_UNCHECKED, 0);
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
			currentDirection = DIRECTION::UP;
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
			currentDirection = DIRECTION::DOWN;
			return 0;

		case IDC_FIND_NEXT:
		{
			FindNext(hwndDlg);
			return 0;
		}

		case IDC_REPLACE:
		{
			ReplaceTextWith(hwndDlg);
			return 0;
		}


		case IDC_REPLACE_ALL:
			ReplaceAll(hwndDlg);
			return 0;

		case IDCANCEL:
			position = nullptr;
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