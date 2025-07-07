#include "pch.h"
#include "Effects/EffectPass.h"
#include "Effects/SobelPass.h"
#include "Effects/EffectManager.h"

void EffectManager::setDevice(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext) {

	_d3dDevice = d3dDevice;
	_d3dContext = d3dContext;
}

void EffectManager::setPixelShaders(ID3D11VertexShader* vertexShader) {

	for (int i = 0; i < EFFECT_COUNT; i++) {

		effects[i]->Initialize(_d3dDevice, _d3dContext, vertexShader);
	}
}

void EffectManager::applyEffects(ID3D11RenderTargetView* renderTargetView) {

	for (int i = 0; i < EFFECT_COUNT; i++) {

		effects[i]->Apply(renderTargetView);
	}
}