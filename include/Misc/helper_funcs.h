#include "resource.h"
namespace Helper {

	std::string ReadShaderFile(const std::string& filename);
	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	INT_PTR CALLBACK MonitorSelectProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
	void AttachConsoleToWindow();
}