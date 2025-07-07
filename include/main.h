#pragma once
#include "Pipeline/D3D11Pipeline.h"
#include "Effects/EffectManager.h"
bool CaptureFrame(D3D11Pipeline& pipeline, EffectManager& effectManager);
void initMainWindow(HINSTANCE* hInstance, HWND* hWnd);
/*
struct Sobel {

    ID3D11PixelShader* greyscaleShader = nullptr;
    ID3D11PixelShader* magnitudeShader = nullptr;
    ID3D11PixelShader* sobelShader = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc = {};
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    ID3D11UnorderedAccessView* magnitudeUAV = nullptr;
    ID3D11UnorderedAccessView* greyscaleUAV = nullptr;
    ID3D11UnorderedAccessView* sobelUAV = nullptr;


};
*/

// Forward declaration of WndProc and compileShader and RenderFrame and CaptureFrame
/*
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MonitorSelectProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
bool InitD3D(HWND hWnd);
void initMainWindow(HINSTANCE* hInstance, HWND* hWnd);
void compileShader();
*/
//void RenderFrame();

/*
std::string ReadShaderFile(const std::string& filename);
void compileShaderFile(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex, int* entryIdx);
void createSamplerState();
void createVertexBuffer();
void createViewport();
*/
/*
// Debug function so whatever ill put it in the header
inline void AttachConsoleToWindow()
{
    // Allocate a new console
    AllocConsole();

    // Redirect std::cout, std::cerr, and std::cin to the console
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);  
    freopen_s(&stream, "CONOUT$", "w", stderr);  
    freopen_s(&stream, "CONIN$", "r", stdin);    
}
*/