#pragma once

using Microsoft::WRL::ComPtr;

class Texture {
public:
	Texture(std::wstring name)
		:
		textureName(name) {
	};

	void Create(const DeviceResources* deviceRes);
	void Apply(const DeviceResources* deviceRes);

private:
	std::wstring textureName;

	ComPtr<ID3D11ShaderResourceView> textureRV;
	ComPtr<ID3D11SamplerState> samplerState;
};
