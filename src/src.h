#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>     // Include DXGI 1.2 only (for desktop duplication)
#include <d3dcompiler.h> // for HLSL
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <direct.h>
#include <DirectXMath.h>
#include <cstdio> // for debugging console

// Link against necessary libraries
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

// Vertex data
struct Vertex {

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texCoord;
};

struct Sobel {

    ID3D11PixelShader* greyscaleShader = nullptr;
    ID3D11PixelShader* magnitudeShader = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc = {};
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    ID3D11UnorderedAccessView* magnitudeUAV = nullptr;
    ID3D11UnorderedAccessView* greyscaleUAV = nullptr;
};


// Forward declaration of WndProc and compileShader and RenderFrame and CaptureFrame
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK MonitorSelectProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
bool InitD3D(HWND hWnd);
void initMainWindow(HINSTANCE* hInstance, HWND* hWnd);
void compileShader();
void RenderFrame();
bool CaptureFrame();
std::string ReadShaderFile(const std::string& filename);
void compileShaderFile(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex, int* entryIdx);
void createSamplerState();
void createVertexBuffer();
void createViewport();

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