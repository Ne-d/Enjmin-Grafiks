#pragma once

#include <iostream>

using Microsoft::WRL::ComPtr;

template <typename TVertex>
class VertexBuffer {
public:
	VertexBuffer() = default;

	uint32_t PushVertex(TVertex v) {
		data.push_back(v);
		return data.size() - 1;
	}

	[[nodiscard]] size_t Size() const {
		return data.size();
	}

	void Clear() {
		data.clear();
	}

	void Create(const DeviceResources* deviceResources) {
		buffer.Reset();
		if (data.size() == 0)
			return;
		
		const CD3D11_BUFFER_DESC desc(sizeof(TVertex) * data.size(), D3D11_BIND_VERTEX_BUFFER);

		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = data.data();

		const HRESULT result = deviceResources->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.GetAddressOf()
		);

		if (result != S_OK) {
			// TODO: Figure out some better way of handling errors and logging.
			std::cerr << "Failed to create vertex buffer" << std::endl;
			exit(1);
		}
	}

	void Apply(const DeviceResources* deviceRes, const int slot = 0) const {
		assert(buffer.Get() != nullptr);
		ID3D11Buffer* vbs[] = { buffer.Get() };
		const UINT strides[] = { sizeof(TVertex) };
		constexpr UINT offsets[] = { 0 };
		deviceRes->GetD3DDeviceContext()->IASetVertexBuffers(slot, 1, vbs, strides, offsets);
	}

private:
	ComPtr<ID3D11Buffer> buffer;
	std::vector<TVertex> data;
};

class IndexBuffer {
public:
	IndexBuffer() = default;

	void PushTriangle(const uint32_t a, const uint32_t b, const uint32_t c) {
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
	}

	[[nodiscard]] size_t Size() const {
		return indices.size();
	}

	void Clear() {
		indices.clear();
	}

	void Create(const DeviceResources* deviceRes) {
		buffer.Reset();
		if (indices.size() == 0)
			return;
		
		const CD3D11_BUFFER_DESC desc(sizeof(uint32_t) * indices.size(), D3D11_BIND_INDEX_BUFFER);

		D3D11_SUBRESOURCE_DATA dataInitial = {};
		dataInitial.pSysMem = indices.data();

		const HRESULT result = deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			&dataInitial,
			buffer.GetAddressOf()
		);

		if (result != S_OK) {
			// TODO: Figure out some better way of handling errors and logging.
			std::cerr << "IndexBuffer::Create: Failed to create index buffer" << std::endl;
			exit(1);
		}
	}
	
	void Apply(const DeviceResources* deviceRes) const {
		assert(buffer.Get() != nullptr);
		deviceRes->GetD3DDeviceContext()->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}

private:
	ComPtr<ID3D11Buffer> buffer;
	std::vector<uint32_t> indices;
};

template <typename TData>
class ConstantBuffer {
public:
	ConstantBuffer() = default;

	void Create(const DeviceResources* deviceRes) {
		const CD3D11_BUFFER_DESC desc(sizeof(TData), D3D11_BIND_CONSTANT_BUFFER);

		const HRESULT result = deviceRes->GetD3DDevice()->CreateBuffer(
			&desc,
			nullptr,
			buffer.ReleaseAndGetAddressOf()
		);

		if (result != S_OK) {
			// TODO: Figure out some better way of handling errors and logging.
			std::cerr << "Failed to create vertex buffer" << std::endl;
			exit(1);
		}
	}

	void UpdateBuffer(const DeviceResources* deviceRes, const TData& data) {
		deviceRes->GetD3DDeviceContext()->UpdateSubresource(buffer.Get(), 0, nullptr, &data, 0, 0);
	}

	void ApplyToVS(const DeviceResources* deviceRes, const int slot = 0) const {
		assert(buffer.Get() != nullptr);
		ID3D11Buffer* cbs[] = { buffer.Get() };
		deviceRes->GetD3DDeviceContext()->VSSetConstantBuffers(slot, 1, cbs);
	}

private:
	ComPtr<ID3D11Buffer> buffer;
};
