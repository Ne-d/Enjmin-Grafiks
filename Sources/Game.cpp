//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include <iostream>

#include "PerlinNoise.hpp"
#include "Engine/Buffers.h"
#include "Minicraft/Cube.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/Camera.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader* basicShader;

struct ModelData {
	Matrix model;
};

ConstantBuffer<ModelData> constantBufferModel;
ComPtr<ID3D11InputLayout> inputLayout;

std::vector<Cube> cubes;
Texture texture(L"terrain");
Camera camera(80, 1);

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2);
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

	GenerateInputLayout<VertexLayout_PositionUV>(m_deviceResources.get(), basicShader);

	texture.Create(m_deviceResources.get());

	camera.UpdateAspectRatio((float)width / (float)height);
	
	int size = 10;
	cubes.reserve(2 * size * size * size);
	for (int x = -size; x < size; ++x) {
		for (int y = -size; y < size; ++y) {
			for (int z = -size; z < size; ++z) {
				auto& cube = cubes.emplace_back(Vector3(x * 2, y * 2, z * 2));
				cube.Generate(m_deviceResources.get());
			}
		}
	}

	constantBufferModel.Create(m_deviceResources.get());
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
	
	// add kb/mouse interact here
	camera.Update(timer.GetElapsedSeconds(), kb, m_mouse.get());
	
	//cube.SetModelMatrix(Matrix::CreateTranslation(0, sin(m_timer.GetTotalSeconds() * 4) * 0.75, 0));
	
	if (kb.Escape)
		ExitGame();

	auto const pad = m_gamePad->GetState(0);
}

// Draws the scene.
void Game::Render() {
	// Don't try to render anything before the first Update.
	if (m_timer.GetFrameCount() == 0)
		return;

	// Setup Device Resource Variables
	const auto context = m_deviceResources->GetD3DDeviceContext();
	const auto renderTarget = m_deviceResources->GetRenderTargetView();
	const auto depthStencil = m_deviceResources->GetDepthStencilView();
	const auto viewport = m_deviceResources->GetScreenViewport();

	// Prepare frame
	context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
	context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	context->RSSetViewports(1, &viewport);
	context->OMSetRenderTargets(1, &renderTarget, depthStencil);
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(inputLayout.Get());
	ApplyInputLayout<VertexLayout_PositionUV>(m_deviceResources.get());
	basicShader->Apply(m_deviceResources.get());

	// Prepare and send Constant Buffers (Model, View and Projection matrices) to Vertex Shader.

	constantBufferModel.ApplyToVS(m_deviceResources.get(), 0);

	texture.Apply(m_deviceResources.get());

	camera.Apply(m_deviceResources.get());
	
	// Draw objects
	for (auto& cube : cubes) {
		const ModelData modelData = ModelData{ cube.GetModelMatrix().Transpose() };
		constantBufferModel.UpdateBuffer(m_deviceResources.get(), modelData);
		cube.Draw(m_deviceResources.get());
	}
	
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

	camera.UpdateAspectRatio((float)width / (float)height);
	
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
