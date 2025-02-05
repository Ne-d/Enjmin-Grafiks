#include "pch.h"
#include "Texture.h"

#include "DDSTextureLoader.h"

void Texture::Create(const DeviceResources* deviceRes) {
	const auto d3dDevice = deviceRes->GetD3DDevice();

	CreateDDSTextureFromFileEx(d3dDevice,
		(std::wstring(L"Textures/Compiled/") + textureName + L".dds").c_str(), 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0,
		DirectX::DDS_LOADER_IGNORE_SRGB, nullptr, textureRV.ReleaseAndGetAddressOf());

	CD3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC(D3D11_DEFAULT);
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
	d3dDevice->CreateSamplerState(&samplerDesc, &samplerState);
}

void Texture::Apply(const DeviceResources* deviceRes) {
	const auto d3dContext = deviceRes->GetD3DDeviceContext();

	ID3D11ShaderResourceView* srv[] = { textureRV.Get() };
	d3dContext->PSSetShaderResources(0, 1, srv);

	ID3D11SamplerState* samplers[] = { samplerState.Get() };
	d3dContext->PSSetSamplers(0, 1, samplers);
}
