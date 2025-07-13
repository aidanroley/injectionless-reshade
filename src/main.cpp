#include "pch.h"
#include "Effects/EffectManager.h"
#include "Pipeline/D3D11Pipeline.h"
#include "Misc/helper_funcs.h"
#include "main.h"


// Main 
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

    int monitorNum = 0;
    int selectedMonitor = DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_MONITOR_SELECT), NULL, Helper::MonitorSelectProc, 0);
    if (selectedMonitor > 0) {

        monitorNum = selectedMonitor - 1; // 0-based idx is returned in selectedMonitor
    }
    else {

        monitorNum = 0;
    }

    HWND hWnd;
    initMainWindow(&hInstance, &hWnd);

    D3D11Pipeline pipeline;
    pipeline.init(hWnd, monitorNum);

    EffectManager effectManager;
    pipeline.setEffectManager(&effectManager);
    pipeline.setVertexShader();

    // Compile shader code and put it in the textures
    pipeline.setPixelShaders();

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    // Message loop
    MSG msg = { 0 };
    while (true) {

        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

            if (msg.message == WM_QUIT) {

                return (int)msg.wParam;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            // Check if CTRL + W is pressed and close window if pressed
            if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState('W') & 0x8000)) {

                SendMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }

        // Render the frame every iteration
        CaptureFrame(pipeline, effectManager);
    }

    // Cleanup (not reached in this case, as loop runs indefinitely)
    //renderTargetView->Release();
    //swapChain->Release();
    //d3dDevice->Release();
    //d3dContext->Release();

    return (int)msg.wParam;
}


void initMainWindow(HINSTANCE* hInstance, HWND* hWnd) {

    // Define and register the window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = Helper::WndProc;
    wc.hInstance = *hInstance;
    wc.lpszClassName = L"NormalWindowClass";
    RegisterClassEx(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create a normal window
    *hWnd = CreateWindowEx(
        0,                                 // No extended styles, regular window
        L"NormalWindowClass",              // Window class name
        L"Regular Window",                 // Window title
        WS_POPUP,                          // Standard window style
        0, 0,                              // Position
        screenWidth, screenHeight,         // Width, Height
        nullptr,                           // Parent window handle
        nullptr,                           // Menu handle
        *hInstance,                         // Application instance handle
        nullptr                            // Additional parameters
    );

    // Make window cover the screen
    SetWindowPos(*hWnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_NOZORDER | SWP_SHOWWINDOW);
    Helper::AttachConsoleToWindow();
}
bool CaptureFrame(D3D11Pipeline& pipeline, EffectManager& effectManager) {

    ID3D11Texture2D* desktopTexture = nullptr;
    if (!pipeline.updateDesktopTexture(&desktopTexture)) return false;

    ID3D11ShaderResourceView* textureSRV = nullptr;
    if (!pipeline.updateDesktopSRV(&textureSRV, desktopTexture)) return false;

    pipeline.setVertexBuffers();
    pipeline.bindSRVSampler(&textureSRV);

    effectManager.applyEffects(pipeline.getRenderTargetView());

    pipeline.Present();
    pipeline.frameCleanup();

    return true;
}