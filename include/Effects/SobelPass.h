#pragma once
#include "EffectPass.h"
class SobelPass : public EffectPass {
public:

    void Initialize(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11VertexShader* vertexShader) override;
    void Apply(ID3D11RenderTargetView* renderTargetView) override;
    const char* GetName() const override { return "Sobel"; }

private:

    void getSobelShaders(ID3D11Device* d3dDevice, std::string sobelShaderSource, int* entryIdx);
    void getSobelUAVs();

    // store each shader function
    ID3D11PixelShader* greyscaleShader;
    ID3D11PixelShader* magnitudeShader;
    ID3D11PixelShader* sobelShader;
    // store rules for texture and the UAVs for read/writing 
    // **consider deleting the desc ones**
    D3D11_TEXTURE2D_DESC textureDesc = {};
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
    ID3D11UnorderedAccessView* magnitudeUAV = nullptr;
    ID3D11UnorderedAccessView* greyscaleUAV = nullptr;

    ID3D11Device* _d3dDevice = nullptr;
    ID3D11DeviceContext* _d3dContext = nullptr;
    ID3D11VertexShader* vertexShader = nullptr;

};