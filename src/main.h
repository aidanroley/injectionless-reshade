#pragma once
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


// Forward declaration of WndProc and compileShader and RenderFrame and CaptureFrame
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
//INT_PTR CALLBACK MonitorSelectProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
//bool InitD3D(HWND hWnd);
void initMainWindow(HINSTANCE* hInstance, HWND* hWnd);
//void compileShader(D3D11Pipeline* pipeline);
void RenderFrame();
//bool CaptureFrame();
//std::string ReadShaderFile(const std::string& filename);
//void compileShaderFile(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex, int* entryIdx);
//void createSamplerState();
//void createVertexBuffer();
//void createViewport();

// Debug function so whatever ill put it in the header
