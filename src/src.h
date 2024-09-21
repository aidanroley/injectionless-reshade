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

// Direct3D Variables
ID3D11Device* d3dDevice = nullptr;
ID3D11DeviceContext* d3dContext = nullptr;
IDXGISwapChain* swapChain = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

// Create DXGI Factory
IDXGIFactory1* dxgiFactory = nullptr;

// Create Adapter and output (monitor)
IDXGIAdapter1* adapter = nullptr;
IDXGIOutput* output = nullptr;

// Create desktop duplication
IDXGIOutput1* output1 = nullptr;
IDXGIOutputDuplication* deskDuplication = nullptr;


// Capture Frame
DXGI_OUTDUPL_FRAME_INFO frameInfo;
IDXGIResource* desktopResource = nullptr;

// For shader compilation
ID3D11PixelShader* pixelShader = nullptr;
ID3D11VertexShader* vertexShader = nullptr;

// For shader
ID3D11InputLayout* inputLayout;
ID3D11SamplerState* samplerState = nullptr;
ID3D11Buffer* vertexBuffer = nullptr;
D3D11_VIEWPORT viewport;

// Vertex data
struct Vertex {

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texCoord;
};

Vertex vertices[] = {

    { DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top-left
    { DirectX::XMFLOAT3( 1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // Top-right
    { DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom-right
    
    { DirectX::XMFLOAT3( 1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom-right
    { DirectX::XMFLOAT3( -1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // Bottom-left
	{ DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top-left
};

D3D11_INPUT_ELEMENT_DESC layout[] = {

    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
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
void compileShaderFile(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex);
void createSamplerState();
void createVertexBuffer();
void createViewport();

int monitorInputIndex;

// Debug function so whatever ill put it in the header
void AttachConsoleToWindow()
{
    // Allocate a new console
    AllocConsole();

    // Redirect std::cout, std::cerr, and std::cin to the console
    FILE* stream;
    freopen_s(&stream, "CONOUT$", "w", stdout);  
    freopen_s(&stream, "CONOUT$", "w", stderr);  
    freopen_s(&stream, "CONIN$", "r", stdin);    
}