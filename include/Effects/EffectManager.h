#pragma once
#include "Effects/EffectPass.h"
#include "Effects/SobelPass.h"
class EffectManager {
public:

	EffectManager() {

		effects[EFFECT_SOBEL] = std::make_unique<SobelPass>();
	}

	void setPixelShaders(ID3D11VertexShader* vertexShader);
	void setDevice(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext);
	void applyEffects(ID3D11RenderTargetView* renderTargetView);

private:

	ID3D11Device* _d3dDevice = nullptr;
	ID3D11DeviceContext* _d3dContext = nullptr;

	std::array<std::unique_ptr<EffectPass>, EFFECT_COUNT> effects;
	std::bitset<EFFECT_COUNT> effectEnabled;
};

