#include "pch.h"
#include "Misc/helper_funcs.h"
#include "Effects/SobelPass.h"

void SobelPass::Apply(ID3D11RenderTargetView* renderTargetView) {

    // bind UAV for writing
    ID3D11UnorderedAccessView* uavs[2] = { greyscaleUAV, magnitudeUAV };
    _d3dContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &renderTargetView, nullptr, 1, 2, uavs, nullptr);


    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };  // Clear to black
    _d3dContext->ClearUnorderedAccessViewFloat(greyscaleUAV, clearColor);
    _d3dContext->ClearUnorderedAccessViewFloat(magnitudeUAV, clearColor);




    // Set vertex and pixel shaders
    _d3dContext->VSSetShader(vertexShader, nullptr, 0);
    _d3dContext->PSSetShader(greyscaleShader, nullptr, 0);

    // draw it. it draws to sobelInstance.greyscaleUAV
    _d3dContext->Draw(6, 0);
    // ******GREYSCALE PASS END ******************

    //*******MAGNITUDE PASS ******************
    // now its done drawing...reading from that texture it drew to and pass it to magnitude shader
    _d3dContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &renderTargetView, nullptr, 1, 2, uavs, nullptr);

    // Set vertex and shader where greyscalePass is compiled in
    _d3dContext->VSSetShader(vertexShader, nullptr, 0);
    _d3dContext->PSSetShader(magnitudeShader, nullptr, 0);
    _d3dContext->Draw(6, 0);
    // **** MAGNITUDE PASS END ***********************


    //
    _d3dContext->VSSetShader(vertexShader, nullptr, 0);
    _d3dContext->PSSetShader(sobelShader, nullptr, 0);
    _d3dContext->Draw(6, 0);
}

void SobelPass::Initialize(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11VertexShader* vertexShader) {

    // set device/context for class
    _d3dContext = d3dContext;
    _d3dDevice = d3dDevice;
    // get shader file source code
    fs::path shaderDir = fs::current_path() / "shaders";
    std::string sobelShaderSource = Helper::ReadShaderFile((shaderDir / "shader.fx").string());
    int entryIdx = 0;
    while (entryIdx < 3) { // –‚–@

        getSobelShaders(d3dDevice, sobelShaderSource, &entryIdx);
        entryIdx++;
    }
}

// creates 2D textures and stores them in UAVs
void SobelPass::getSobelUAVs() {

    textureDesc.Width = 1920;
    textureDesc.Height = 1080;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // float3
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* greyscaleTexture = nullptr;

    HRESULT hr = _d3dDevice->CreateTexture2D(&textureDesc, nullptr, &greyscaleTexture);
    if (FAILED(hr)) {
        std::cerr << "Failed to create greyscale texture (sobel)." << std::endl;
        return;
    }

    uavDesc.Format = textureDesc.Format;
    uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    hr = _d3dDevice->CreateUnorderedAccessView(greyscaleTexture, &uavDesc, &greyscaleUAV);
    if (FAILED(hr)) {
        std::cerr << "Failed to create UAV for greyscale texture(sobel)." << std::endl;
        return;
    }

    greyscaleTexture->Release(); // Because these 2 textures are in unordered acces views, they are bound once set, you don't need to update every frame (unlike shaderTexture)

    ID3D11Texture2D* magnitudeTexture = nullptr;

    hr = _d3dDevice->CreateTexture2D(&textureDesc, nullptr, &magnitudeTexture);
    if (FAILED(hr)) {
        std::cerr << "Failed to create magnitude texture (sobel)." << std::endl;
        return;
    }

    hr = _d3dDevice->CreateUnorderedAccessView(magnitudeTexture, &uavDesc, &magnitudeUAV);
    if (FAILED(hr)) {
        std::cerr << "Failed to create UAV for magnitude texture(sobel)." << std::endl;
        return;
    }

    magnitudeTexture->Release();
}

void SobelPass::getSobelShaders(ID3D11Device* d3dDevice, std::string sobelShaderSource, int* entryIdx) {

    static const char* entryPoints[] = { "greyscalePass", "blurPass", "applySobel" };

    if (*entryIdx < 0 || *entryIdx >= 3) {
        std::cerr << "Invalid entry index: " << *entryIdx << std::endl;
        ExitProcess(1);
    }

    const char* entryPoint = entryPoints[*entryIdx];

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompile(
        sobelShaderSource.c_str(),
        sobelShaderSource.length(),
        nullptr,
        nullptr,
        nullptr,
        entryPoint,
        "ps_5_0",
        0,
        0,
        &shaderBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            std::cerr << "Pixel shader comp. error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
            errorBlob->Release();
        }
        if (shaderBlob) shaderBlob->Release();
        ExitProcess(1);
    }

    switch (*entryIdx) {
    case 0:
        hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &greyscaleShader);
        break;
    case 1:
        hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &magnitudeShader);
        break;
    case 2:
        hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &sobelShader);
        break;
    }

    if (FAILED(hr)) {
        std::cerr << "Failed to create pixel shader for sobel" << std::endl;
        shaderBlob->Release();
        ExitProcess(1);
    }

    shaderBlob->Release();
}
