//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <iostream>

#include "PerlinNoise.hpp"
#include "Engine/Shader.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;

ComPtr<ID3D11Buffer> vertexBuffer;
ComPtr<ID3D11Buffer> indexBuffer;
ComPtr<ID3D11Buffer> constantBufferModel;
ComPtr<ID3D11Buffer> constantBufferCamera;
ComPtr<ID3D11InputLayout> inputLayout;

struct ModelData {
	Matrix model;
};

struct CameraData {
	Matrix view;
	Matrix projection;
};

// TODO: Put this in some Camera class
Matrix view;
Matrix projection;

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT, 2);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	delete basicShader;
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, const int width, const int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	basicShader = new Shader(L"Basic");
	basicShader->Create(m_deviceResources.get());

	auto device = m_deviceResources->GetD3DDevice();

	const std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementDescs = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	device->CreateInputLayout(
		InputElementDescs.data(), InputElementDescs.size(),
		basicShader->vsBytecode.data(), basicShader->vsBytecode.size(),
		inputLayout.ReleaseAndGetAddressOf());

	// TP: allouer vertexBuffer ici
	const std::vector<float> vertices = {
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f
	};

	const std::vector<uint32_t> indices = {
		0, 1, 2,
		2, 3, 0
	};

	// Vertex Buffer
	D3D11_SUBRESOURCE_DATA vertexSubresourceData;
	vertexSubresourceData.pSysMem = vertices.data();

	const CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(float) * vertices.size(), D3D11_BIND_VERTEX_BUFFER);
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, &vertexSubresourceData, vertexBuffer.ReleaseAndGetAddressOf());
	if (result != S_OK) {
		std::cerr << "Failed to create vertex buffer" << std::endl;
		exit(1);
	}

	// Index Buffer
	D3D11_SUBRESOURCE_DATA indexSubresourceData;
	indexSubresourceData.pSysMem = indices.data();

	const CD3D11_BUFFER_DESC indexBufferDesc(sizeof(uint32_t) * indices.size(), D3D11_BIND_INDEX_BUFFER);
	result = device->CreateBuffer(&indexBufferDesc, &indexSubresourceData, indexBuffer.ReleaseAndGetAddressOf());
	if (result != S_OK) {
		std::cerr << "Failed to create index buffer" << std::endl;
		exit(1);
	}

	// Matrix Constant Buffer
	const CD3D11_BUFFER_DESC modelConstantBufferDesc(sizeof(ModelData), D3D11_BIND_CONSTANT_BUFFER);
	result = device->CreateBuffer(&modelConstantBufferDesc, nullptr, constantBufferModel.ReleaseAndGetAddressOf());
	if (result != S_OK) {
		std::cerr << "Failed to create model constant buffer" << std::endl;
		exit(1);
	}
	
	const CD3D11_BUFFER_DESC cameraConstantBufferDesc(sizeof(CameraData), D3D11_BIND_CONSTANT_BUFFER);
	result = device->CreateBuffer(&cameraConstantBufferDesc, nullptr, constantBufferCamera.ReleaseAndGetAddressOf());
	if (result != S_OK) {
		std::cerr << "Failed to create camera constant buffer" << std::endl;
		exit(1);
	}
	
	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(90),
		(float)width / (float)height,
		0.1f,
		100.0f
	);
}

void Game::Tick() {
	// DX::StepTimer will compute the elapsed time and call Update() for us
	// We pass Update as a callback to Tick() because StepTimer can be set to a "fixed time" step mode, allowing us to call Update multiple time in a row if the framerate is too low (useful for physics stuffs)
	m_timer.Tick([&]() { Update(m_timer); });

	Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer) {
	auto const kb = m_keyboard->GetState();
	auto const ms = m_mouse->GetState();
	
	// add kb/mouse interact here
	view = Matrix::CreateLookAt(
		Vector3(sin(m_timer.GetTotalSeconds()), 0, cos(m_timer.GetTotalSeconds())),
		Vector3::Zero,
		Vector3::Up
	);
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	const auto context = m_deviceResources->GetD3DDeviceContext();
	const auto renderTarget = m_deviceResources->GetRenderTargetView();
	const auto depthStencil = m_deviceResources->GetDepthStencilView();
	const auto viewport = m_deviceResources->GetScreenViewport();

	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());

	basicShader->Apply(m_deviceResources.get());

	// TP: Tracer votre vertex buffer ici
	ID3D11Buffer* vertexBuffers[] = { vertexBuffer.Get() };
	constexpr UINT strides[] = { sizeof(float) * 3 };
	constexpr UINT offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, vertexBuffers, strides, offsets);
	
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	const ModelData modelData = ModelData{
		Matrix::CreateTranslation(0, sin(m_timer.GetTotalSeconds() * 4) / 4, 0).Transpose()
	};

	const CameraData cameraData = CameraData{
		view.Transpose(),
		projection.Transpose()
	};

	context->UpdateSubresource(constantBufferModel.Get(), 0, nullptr, &modelData, 0, 0);
	context->UpdateSubresource(constantBufferCamera.Get(), 0, nullptr, &cameraData, 0, 0);

	// Tell the Vertex Shader to use our constant buffers
	ID3D11Buffer* constantBuffers[] = { constantBufferModel.Get(), constantBufferCamera.Get() };
	context->VSSetConstantBuffers(0, 2, constantBuffers);
	
	context->DrawIndexed(6, 0, 0);

	// envoie nos commandes au GPU pour être affiché à l'écran
	m_deviceResources->Present();
}


#pragma region Message Handlers
void Game::OnActivated() {}

void Game::OnDeactivated() {}

void Game::OnSuspending() {}

void Game::OnResuming() {
	m_timer.ResetElapsedTime();
}

void Game::OnWindowMoved() {
	auto const r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnDisplayChange() {
	m_deviceResources->UpdateColorSpace();
}

void Game::OnWindowSizeChanged(int width, int height) {
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	projection = Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(90),
		(float)width / (float)height,
		0.1f,
		100.0f
	);
	
	// The windows size has changed:
	// We can realloc here any resources that depends on the target resolution (post-processing, etc.)
}

void Game::OnDeviceLost() {
	// We have lost the graphics card, we should reset resources [TODO]
}

void Game::OnDeviceRestored() {
	// We have a new graphics card context, we should realloc resources [TODO]
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept {
	width = 800;
	height = 600;
}

#pragma endregion
