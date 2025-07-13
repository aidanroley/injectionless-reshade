#pragma once
#include "types.h"

class D3D11Pipeline {
public:

	bool init(HWND hWnd, int monitorNum);
	void compileShaderFiles(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex, int* entryIdx);
	void setPixelShaders();
	void setEffectManager(EffectManager* effectManager);
	void setVertexShader();

	// after setup functions
	bool updateDesktopTexture(ID3D11Texture2D** desktopTexture);
	bool updateDesktopSRV(ID3D11ShaderResourceView** textureSRV, ID3D11Texture2D* desktopTexture);
	void bindSRVSampler(ID3D11ShaderResourceView** textureSRV);
	void setVertexBuffers();
	void frameCleanup();
	void Present();

	// get
	ID3D11RenderTargetView* getRenderTargetView() {
		return renderTargetView;
	}


private:

	void initData();

	void setupSwapChain(HWND hWnd);
	void setupRenderTarget();
	void setupDesktopDuplication(int monitorNum);

	void createSamplerState();
	void createVertexBuffer();
	void createViewport();

	std::array<ID3D11VertexShader*, EFFECT_COUNT> pixelShaders;

	EffectManager* _effectManager;

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

	// resource view. (for desktop texture)
	ID3D11Texture2D* desktopTexture = nullptr;
	ID3D11ShaderResourceView* textureSRV = nullptr;

	Vertex vertices[6];
	D3D11_INPUT_ELEMENT_DESC layout[2];
};
