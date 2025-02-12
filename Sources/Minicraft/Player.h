#pragma once
#include "Cube.h"

#include "World.h"

#include "Engine/Camera.h"

class Player {
public:
	Player(World* w, Vector3 pos)
		:
		world(w), position(pos) {
	}

	void GenerateGPUResources(DeviceResources* deviceRes);
	void Update(float dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& ms);
	void Draw(DeviceResources* deviceRes);

	PerspectiveCamera* GetCamera() { return &camera; }

private:
	World* world = nullptr;

	Vector3 position = Vector3();
	float velocityY = 0.0f;

	float walkSpeed = 10.0f;

	PerspectiveCamera camera = PerspectiveCamera(75, 1);

	Cube currentCube = Cube(WOOD);
	Cube highlightCube = Cube(HIGHLIGHT);

	DirectX::Mouse::ButtonStateTracker mouseTracker;
	DirectX::Keyboard::KeyboardStateTracker keyboardTracker;
};
