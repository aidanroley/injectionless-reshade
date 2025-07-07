#include "pch.h"
#include "Misc/helper_funcs.h"

namespace Helper {

    std::string ReadShaderFile(const std::string& filename) {

        std::ifstream file(filename);
        if (!file.is_open()) {

            MessageBoxW(nullptr, (L"Failed to open shader file: " + std::wstring(filename.begin(), filename.end())).c_str(), L"Error", MB_OK | MB_ICONERROR);
            return "";  // Return empty string if file cannot be opened
        }

        std::stringstream buffer;
        buffer << file.rdbuf();  // Read the file's content into a stringstream
        return buffer.str();  // Return the contents of the file
    }

    // Window procedure to handle messages
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

        switch (message) {

        case WM_DESTROY:

            PostQuitMessage(0);
            break;

        default:

            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    INT_PTR CALLBACK MonitorSelectProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

        switch (message) {
        case WM_INITDIALOG: {

            // Center the dialog on the screen
            RECT rc;
            GetWindowRect(hDlg, &rc);
            int screenWidth = GetSystemMetrics(SM_CXSCREEN);
            int screenHeight = GetSystemMetrics(SM_CYSCREEN);
            int xPos = (screenWidth - (rc.right - rc.left)) / 2;
            int yPos = (screenHeight - (rc.bottom - rc.top)) / 2;
            SetWindowPos(hDlg, 0, xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

            return (INT_PTR)TRUE;
        }

        case WM_COMMAND: {

            if (LOWORD(wParam) == IDOK) {

                char monitorIndex[10];
                GetDlgItemTextA(hDlg, IDC_EDIT_MONITOR, monitorIndex, sizeof(monitorIndex));

                // Assuming valid input, close dialog and return the selected monitor index
                EndDialog(hDlg, atoi(monitorIndex));
                return (INT_PTR)TRUE;
            }

            if (LOWORD(wParam) == IDCANCEL) {
                // Close the dialog when the user clicks "Cancel" or presses the close button (X)
                EndDialog(hDlg, 0);  // 0 means no monitor selected or canceled
                return (INT_PTR)TRUE;
            }
            break;
        }

        case WM_CLOSE: {
            EndDialog(hDlg, 0);  // Handle the close (X) button
            return (INT_PTR)TRUE;
        }
        }
        return (INT_PTR)FALSE;
    }

    void AttachConsoleToWindow() {

        // Allocate a new console
        AllocConsole();

        // Redirect std::cout, std::cerr, and std::cin to the console
        FILE* stream;
        freopen_s(&stream, "CONOUT$", "w", stdout);
        freopen_s(&stream, "CONOUT$", "w", stderr);
        freopen_s(&stream, "CONIN$", "r", stdin);
    }
}
