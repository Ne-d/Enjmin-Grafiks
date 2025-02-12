//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

#include "Engine/BlendState.h"
#include "Engine/DepthState.h"
#include "Engine/Shader.h"
#include "Engine/Texture.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/Player.h"
#include "Minicraft/World.h"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

// Global stuff
Shader opaqueShader(L"Basic");
Shader transparentShader(L"Water");
Shader waterShader(L"Water");

BlendState blendStateOpaque;
BlendState blendStateTransparent(D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD,
	D3D11_BLEND_ONE, D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_OP_ADD);

DepthState depthStateOpaque(true, true);
DepthState depthStateTransparent(true, false);

ComPtr<ID3D11InputLayout> inputLayout;

World world(16, 8, 16);
Texture texture(L"terrain");
Player player(&world, Vector3(16, 64, 16));

// Game
Game::Game() noexcept(false) {
	m_deviceResources = std::make_unique<DeviceResources>(DXGI_FORMAT_B8G8R8A8_UNORM_SRGB, DXGI_FORMAT_D32_FLOAT, 2,
		D3D_FEATURE_LEVEL_11_0, DeviceResources::c_FlipPresent/* | DeviceResources::c_AllowTearing*/);
	m_deviceResources->RegisterDeviceNotify(this);
}

Game::~Game() {
	g_inputLayouts.clear();
}

void Game::Initialize(HWND window, const int width, const int height) {
	// Create input devices
	m_gamePad = std::make_unique<GamePad>();
	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(window);
	m_mouse->SetMode(Mouse::MODE_RELATIVE);

	// Initialize the Direct3D resources
	m_deviceResources->SetWindow(window, width, height);
	m_deviceResources->CreateDeviceResources();
	m_deviceResources->CreateWindowSizeDependentResources();

	opaqueShader.Create(m_deviceResources.get());
	transparentShader.Create(m_deviceResources.get());
	waterShader.Create(m_deviceResources.get());

	blendStateOpaque.Create(m_deviceResources.get());
	blendStateTransparent.Create(m_deviceResources.get());
	depthStateOpaque.Create(m_deviceResources.get());
	depthStateTransparent.Create(m_deviceResources.get());

	GenerateInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get(), &opaqueShader);

	texture.Create(m_deviceResources.get());

	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);

	world.Generate(m_deviceResources.get());
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
	player.Update(timer.GetElapsedSeconds(), kb, ms);
	
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
	ApplyInputLayout<VertexLayout_PositionNormalUV>(m_deviceResources.get());
	
	texture.Apply(m_deviceResources.get());
	player.GetCamera()->Apply(m_deviceResources.get());

	// Opaque render pass
	blendStateOpaque.Apply(m_deviceResources.get());
	depthStateOpaque.Apply(m_deviceResources.get());
	opaqueShader.Apply(m_deviceResources.get());
	world.Draw(player.GetCamera(), m_deviceResources.get(), RenderPass_Opaque);

	// Transparent render pass
	blendStateTransparent.Apply(m_deviceResources.get());
	depthStateTransparent.Apply(m_deviceResources.get());
	transparentShader.Apply(m_deviceResources.get());
	world.Draw(player.GetCamera(), m_deviceResources.get(), RenderPass_Transparent);

	waterShader.Apply(m_deviceResources.get());
	world.Draw(player.GetCamera(), m_deviceResources.get(), RenderPass_Water);
	
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

	player.GetCamera()->UpdateAspectRatio((float)width / (float)height);
	
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
