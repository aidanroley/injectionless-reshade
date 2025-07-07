#include "pch.h"
#include "Misc/helper_funcs.h"
#include "Effects/EffectManager.h"
#include "Pipeline/D3D11Pipeline.h"

// Initialize Direct3D
bool D3D11Pipeline::init(HWND hWnd, int monitorNum) {

    initData();
    setupSwapChain(hWnd);
    setupRenderTarget();
    setupDesktopDuplication(monitorNum);

    

    

    createSamplerState();
    createVertexBuffer();
    createViewport();

    return true;
}

void D3D11Pipeline::initData() {

    Vertex verticesT[] = {

        { DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top-left
        { DirectX::XMFLOAT3(1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) }, // Top-right
        { DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom-right

        { DirectX::XMFLOAT3(1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }, // Bottom-right
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }, // Bottom-left
        { DirectX::XMFLOAT3(-1.0f,  1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) }, // Top-left
    };

    D3D11_INPUT_ELEMENT_DESC layoutT[] = {

        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(DirectX::XMFLOAT3), D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    memcpy(vertices, verticesT, sizeof(verticesT));
    memcpy(layout, layoutT, sizeof(layoutT));

}

void D3D11Pipeline::setVertexShader() {

    fs::path shaderDir = fs::current_path() / "shaders";
    std::string vertexSource = Helper::ReadShaderFile((shaderDir / "vertex.fx").string());

    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    HRESULT hr = D3DCompile(
        vertexSource.c_str(),           // Shader source
        vertexSource.length(),          // Source length
        nullptr,                        // Source name for errors
        nullptr,                        // No defines
        nullptr,                        // No includes
        "VS_Main",                      // Entry function name
        "vs_5_0",                       // Vertex Shader 5.0
        0,                              // Compile options
        0,                              // Effect options
        &shaderBlob,                    // Shader output
        &errorBlob                      // Error output
    );

    if (FAILED(hr)) {

        if (errorBlob) {

            std::cerr << "vertex shader comp. error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
            errorBlob->Release();
        }

        if (shaderBlob) {

            shaderBlob->Release();
        }
        ExitProcess(1);
    }

    // Create vertex shader and input layout
    hr = d3dDevice->CreateVertexShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        nullptr,
        &vertexShader
    );

    hr = d3dDevice->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        &inputLayout
    );

    shaderBlob->Release();
}


void D3D11Pipeline::setupSwapChain(HWND hWnd) {

    // Set up swap chain description
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    // Create Direct3D device, context, and swap chain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(

        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &swapChain,
        &d3dDevice,
        nullptr,
        &d3dContext
    );

    if (FAILED(hr)) {

        std::cerr << "Failed to create D3D11 device and swap chain." << std::endl;
    }
}

void D3D11Pipeline::setupDesktopDuplication(int monitorNum) {

    HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);
    hr = dxgiFactory->EnumAdapters1(0, &adapter);  // First adapter (GPU)
    adapter->EnumOutputs(monitorNum, &output);  // This selects which monitor input is taken from
    output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1); // Query for necessary interface
    output1->DuplicateOutput(d3dDevice, &deskDuplication);
}

void D3D11Pipeline::setupRenderTarget() {

    // Get the back buffer and create a render target view
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    if (FAILED(hr)) {

        std::cerr << "Failed to get the back buffer." << std::endl;
    }

    hr = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);

    // This is now managed by the program, I don't need the reference to it anymore.
    backBuffer->Release();

    if (FAILED(hr)) {

        std::cerr << "Failed to create render target view." << std::endl;
    }

    // Set the render target
    d3dContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
}

void D3D11Pipeline::createSamplerState() {

    // Create and bind a sampler state once during initialization
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = d3dDevice->CreateSamplerState(&sampDesc, &samplerState);

    if (FAILED(hr)) {

        std::cerr << "Failed to create sampler state" << std::endl;
    }

}

void D3D11Pipeline::createVertexBuffer() {

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT; // Read only btw
    bufferDesc.ByteWidth = sizeof(vertices);
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Bind as a vertex buffer
    bufferDesc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;  // Point to vertex data

    HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create vertex buffer" << std::endl;
        ExitProcess(1);
    }
}

void D3D11Pipeline::createViewport() {

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(1920);   // Set this to your window's width (whatever the game is running at)
    viewport.Height = static_cast<float>(1080); // Set this to your window's height
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    d3dContext->RSSetViewports(1, &viewport);
}
void D3D11Pipeline::setPixelShaders() {

    _effectManager->setDevice(d3dDevice, d3dContext);
    _effectManager->setPixelShaders(vertexShader);
}
/*
void D3D11Pipeline::compileShaderFiles(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex, int* entryIdx) {

    const char* entryPoint = (*entryIdx == 0) ? "greyscalePass" :
        (*entryIdx == 1) ? "blurPass" :
        (*entryIdx == 2) ? "applySobel" :
        nullptr;

    // Blob = compiled bytecode of HLSL
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    // Select whether it's a vertex or pixel shader based on isVertex
    const char* compilerVersion = isVertex ? "vs_5_0" : "ps_5_0";

    HRESULT hr = E_FAIL;
    // Compile the Shader
    if (entryPoint) {
        hr = D3DCompile(

            shaderSource.c_str(),       // Shader source
            shaderSource.length(),      // Source length
            nullptr,                    // Source name for errors
            nullptr,                    // No defines so nullptr
            nullptr,                    // Includes 
            isVertex ? "VS_Main" : entryPoint,                  // Entry function name
            compilerVersion,            // Pixel Shader 5.0
            0,                          // Shader compile options
            0,                          // Effect options
            &shaderBlob,                // Shader output
            &errorBlob                  // For errors
        );
    }

    if (FAILED(hr) && !isVertex) {

        if (errorBlob) {

            std::cerr << "Shader comp. error: " << (char*)errorBlob->GetBufferPointer() << std::endl;
            errorBlob->Release();
        }

        if (shaderBlob) {

            shaderBlob->Release();
        }

        ExitProcess(1);
    }

    if (!isVertex) {

        if (!isVertex) {

            if (*entryIdx == 0) {

                hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &sobelInstance.greyscaleShader);
            }
            else if (*entryIdx == 1) {

                hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &sobelInstance.magnitudeShader);
            }
            else if (*entryIdx == 2) {

                hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, &sobelInstance.sobelShader);
            }
        }


    }
    else {

        hr = d3dDevice->CreateVertexShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, vertexTexture);
        hr = d3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), &inputLayout); // input layer creaton happens w/ vertex texture
    }


    if (FAILED(hr)) {

        std::cerr << "Failed to create pixel/vertex shader" << std::endl;
        shaderBlob->Release();
        ExitProcess(1);
    }

    shader
    Blob->Release();
}
*/
void D3D11Pipeline::setEffectManager(EffectManager* effectManager) {

    _effectManager = effectManager;
}

void D3D11Pipeline::updateDesktopTexture(ID3D11Texture2D* desktopTexture) {

    HRESULT hr = deskDuplication->AcquireNextFrame(500, &frameInfo, &desktopResource);
    desktopResource->Release();
    if (FAILED(hr)) {

        std::cerr << "Failed to get next frame texture data" << std::endl;
    }

    // Fetch texture from resource
    hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&desktopTexture);

    if (FAILED(hr)) {

        std::cerr << "Failed to get desktop texture";
    }

}

void D3D11Pipeline::updateDesktopSRV(ID3D11ShaderResourceView* textureSRV, ID3D11Texture2D* desktopTexture) {

    HRESULT hr = d3dDevice->CreateShaderResourceView(desktopTexture, nullptr, &textureSRV);
    //desktopTexture->Release();

    if (FAILED(hr)) {
        std::cerr << "Failed to create shader resource view" << std::endl;
    }
}

void D3D11Pipeline::setVertexBuffers() {

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    // pipeline stages
    d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    d3dContext->IASetInputLayout(inputLayout);
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3D11Pipeline::bindSRVSampler() {

    // bind srv for desktop texture for reading in pixel shader (fetched through tex.Sample in the shader)
    d3dContext->PSSetShaderResources(0, 1, &textureSRV);
    d3dContext->PSSetSamplers(0, 1, &samplerState);
}



void D3D11Pipeline::Present() {

    swapChain->Present(1, 0);

}

void D3D11Pipeline::frameCleanup() {

    if (desktopTexture) {

        desktopTexture->Release();
        desktopTexture = nullptr;
    }

    if (desktopResource) {

        desktopResource->Release();
        desktopResource = nullptr;
    }

    if (textureSRV) {

        textureSRV->Release();
        textureSRV = nullptr;
    }
    deskDuplication->ReleaseFrame();
}

