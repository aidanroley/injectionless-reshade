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
#include <filesystem>
#include <bitset>
#include <array>

// usually don't put my own stuff in pch but this is at the very bottom of include heirarchy in all cases so whatever
#include "types.h"

// Link against necessary libraries
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

namespace fs = std::filesystem;