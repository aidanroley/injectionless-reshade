#include "src.h"

// Initialize Direct3D
bool InitD3D(HWND hWnd) {

    AttachConsoleToWindow();

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
        return false;
    }

    // Get the back buffer and create a render target view
    ID3D11Texture2D* backBuffer = nullptr;
    hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);

    if (FAILED(hr)) {

        std::cerr << "Failed to get the back buffer." << std::endl;
        return false;
    }

    hr = d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);

    // This is now manatged by the program, I don't need the reference to it anymore.
    backBuffer->Release();

    if (FAILED(hr)) {

        std::cerr << "Failed to create render target view." << std::endl;
        return false;
    }

    // Set the render target
    d3dContext->OMSetRenderTargets(1, &renderTargetView, nullptr);

    hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgiFactory);
    hr = dxgiFactory->EnumAdapters1(0, &adapter);  // First adapter (GPU)
    adapter->EnumOutputs(0, &output);  // First monitor
    output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&output1); // Query for necessary interface
    output1->DuplicateOutput(d3dDevice, &deskDuplication); 

    createSamplerState();
    createVertexBuffer();
    createViewport();
    
    // Compile shader code and put it in the textures
    compileShader();

    return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

    // Define and register the window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"NormalWindowClass";
    RegisterClassEx(&wc);

    // Create a normal window
    HWND hWnd = CreateWindowEx(
        0,                                 // No extended styles, regular window
        L"NormalWindowClass",              // Window class name
        L"Regular Window",                 // Window title
        WS_OVERLAPPEDWINDOW,               // Standard window style
        CW_USEDEFAULT, CW_USEDEFAULT,      // Position
        1920, 1080,                        // Width, Height
        nullptr,                           // Parent window handle
        nullptr,                           // Menu handle
        hInstance,                         // Application instance handle
        nullptr                            // Additional parameters
    );

    // Initialize Direct3D for the window
    if (!InitD3D(hWnd)) {
        return -1;
    }

    // Show the window with activation
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
        }

        // Render the frame every iteration
        CaptureFrame();
    }

    // Cleanup (not reached in this case, as loop runs indefinitely)
    renderTargetView->Release();
    swapChain->Release();
    d3dDevice->Release();
    d3dContext->Release();

    return (int)msg.wParam;
}

// Window procedure to handle messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {

    case WM_DESTROY:

        PostQuitMessage(0);
        break;

    default:

        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool CaptureFrame() {

    HRESULT hr = deskDuplication->AcquireNextFrame(0, &frameInfo, &desktopResource);
    if (FAILED(hr)) {

        std::cerr << "Failed to get next frame texture data" << std::endl;
        return false;
    }

    // Fetch texture from resource
    ID3D11Texture2D* desktopTexture = nullptr;
    hr = desktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&desktopTexture);
    desktopResource->Release();

    if(FAILED(hr)) {

        std::cerr << "Failed to get desktop texture";
    }

    // Create shader resource view for the texture
    ID3D11ShaderResourceView* textureSRV = nullptr;

    if (textureSRV) {

        textureSRV->Release();
    }

    hr = d3dDevice->CreateShaderResourceView(desktopTexture, nullptr, &textureSRV);
    desktopTexture->Release();

    if (FAILED(hr)) {

        std::cerr << "Failed to create shader resource view" << std::endl;
        return false;
    }

    // Bind vertex buffer, input layout, etc
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    d3dContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);  
    d3dContext->IASetInputLayout(inputLayout);                              
    d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3dContext->VSSetShader(vertexShader, nullptr, 0);
    d3dContext->PSSetShader(pixelShader, nullptr, 0);

    // Bind the texture to pixel shader
    d3dContext->PSSetShaderResources(0, 1, &textureSRV);
    d3dContext->PSSetSamplers(0, 1, &samplerState);


    d3dContext->Draw(6, 0); 
    
    RenderFrame();


    // Show the frame, release it
    deskDuplication->ReleaseFrame();

    return true;
}

void compileShader() {

    std::string pixelShaderSource;
    pixelShaderSource = ReadShaderFile("C:\\Users\\Aidan\\source\\repos\\injectionless-post-process\\shaders\\shader.fx");
    std::string vertexShaderSource = ReadShaderFile("C:\\Users\\Aidan\\source\\repos\\injectionless-post-process\\shaders\\vertex.fx");

    bool isVertex = false; 
    compileShaderFile(pixelShaderSource, &pixelShader, &vertexShader, isVertex);
    isVertex = true;
    compileShaderFile(vertexShaderSource, &pixelShader, &vertexShader, isVertex);

}
void RenderFrame() {

    swapChain->Present(1, 0);  // Present with vsync
}

std::string ReadShaderFile(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {

        MessageBoxW(nullptr, (L"Failed to open shader file: " + std::wstring(filename.begin(), filename.end())).c_str(), L"Error", MB_OK | MB_ICONERROR);
        return "";  // Return empty string if file cannot be opened
    }

    std::stringstream buffer;
    buffer << file.rdbuf();  // Read the file's content into a stringstream
    return buffer.str();  // Return the contents of the file
}

void createSamplerState() {

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

void createVertexBuffer() {

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;             // Read only btw
    bufferDesc.ByteWidth = sizeof(vertices);            
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;    // Bind as a vertex buffer
    bufferDesc.CPUAccessFlags = 0;                      

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;  // Point to vertex data

    HRESULT hr = d3dDevice->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create vertex buffer" << std::endl;
        ExitProcess(1);
    }
}

void createViewport() {

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(640);   // Set this to your window's width (whatever the game is running at)
    viewport.Height = static_cast<float>(480); // Set this to your window's height
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    d3dContext->RSSetViewports(1, &viewport);
}

void compileShaderFile(std::string shaderSource, ID3D11PixelShader** shaderTexture, ID3D11VertexShader** vertexTexture, bool isVertex) {

    // Blob = compiled bytecode of HLSL
    ID3DBlob* shaderBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;

    // Select whether it's a vertex or pixel shader based on isVertex
    const char* compilerVersion = isVertex ? "vs_5_0" : "ps_5_0";

    // Compile the Shader
    HRESULT hr = D3DCompile(

        shaderSource.c_str(),       // Shader source
        shaderSource.length(),      // Source length
        nullptr,                    // Source name for errors
        nullptr,                    // No defines so nullptr
        nullptr,                    // Includes 
        isVertex ? "VS_Main" : "PS_Main",                  // Entry function name
        compilerVersion,            // Pixel Shader 5.0
        0,                          // Shader compile options
        0,                          // Effect options
        &shaderBlob,                // Shader output
        &errorBlob                  // For errors
    );

    if (FAILED(hr)) {

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

        hr = d3dDevice->CreatePixelShader(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), nullptr, shaderTexture);
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

    shaderBlob->Release();
}

