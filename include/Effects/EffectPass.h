#pragma once
class EffectPass {
public:

	virtual void Initialize(ID3D11Device* d3dDevice, ID3D11DeviceContext* d3dContext, ID3D11VertexShader* vertexShader) = 0;
	virtual void Apply(ID3D11RenderTargetView* renderTargetView) = 0;
	virtual const char* GetName() const = 0;
	virtual ~EffectPass() {}
};