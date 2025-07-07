#pragma once
// Vertex data
struct Vertex {

    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texCoord;
};

enum EffectID {

	EFFECT_SOBEL = 0,
	EFFECT_COUNT // this is the previous one + 1, ...so the amount of effects
	// add more later..
};