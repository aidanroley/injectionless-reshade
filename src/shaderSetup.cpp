#include "shaderSetup.h"

void initSobelShader(ID3D11DeviceContext* d3dContext, ID3D11Device* d3dDevice, Sobel* sobelInstance) {

	sobelInstance->textureDesc.Width = 1920;
	sobelInstance->textureDesc.Height = 1080;
	sobelInstance->textureDesc.MipLevels = 1;
	sobelInstance->textureDesc.ArraySize = 1;
	sobelInstance->textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; // float3
	sobelInstance->textureDesc.SampleDesc.Count = 1;
	sobelInstance->textureDesc.Usage = D3D11_USAGE_DEFAULT;
	sobelInstance->textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

	ID3D11Texture2D* greyscaleTexture = nullptr;

	HRESULT hr = d3dDevice->CreateTexture2D(&sobelInstance->textureDesc, nullptr, &greyscaleTexture);
	if (FAILED(hr)) {
		std::cerr << "Failed to create greyscale texture (sobel)." << std::endl;
		return;
	}

	sobelInstance->uavDesc.Format = sobelInstance->textureDesc.Format;
	sobelInstance->uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	hr = d3dDevice->CreateUnorderedAccessView(greyscaleTexture, &sobelInstance->uavDesc, &sobelInstance->greyscaleUAV);
	if (FAILED(hr)) {
		std::cerr << "Failed to create UAV for greyscale texture(sobel)." << std::endl;
		return;
	}

	greyscaleTexture->Release(); // Because these 2 textures are in unordered acces views, they are bound once set, you don't need to update every frame (unlike shaderTexture)

	ID3D11Texture2D* magnitudeTexture = nullptr;

	hr = d3dDevice->CreateTexture2D(&sobelInstance->textureDesc, nullptr, &magnitudeTexture);
	if (FAILED(hr)) {
		std::cerr << "Failed to create magnitude texture (sobel)." << std::endl;
		return;
	}

	hr = d3dDevice->CreateUnorderedAccessView(magnitudeTexture, &sobelInstance->uavDesc, &sobelInstance->magnitudeUAV);
	if (FAILED(hr)) {
		std::cerr << "Failed to create UAV for magnitude texture(sobel)." << std::endl;
		return;
	}

	magnitudeTexture->Release();

	ID3D11UnorderedAccessView* uavs[] = { sobelInstance->greyscaleUAV, sobelInstance->magnitudeUAV };

	// Bind the UAVs to the pixel shader using the output merger
	d3dContext->OMSetRenderTargetsAndUnorderedAccessViews(
		
    	0,                   
    	nullptr,             
   	 	nullptr,             
    	0,                   
    	2,                   
    	uavs,                // Array of UAVs
    	nullptr              
	);
}