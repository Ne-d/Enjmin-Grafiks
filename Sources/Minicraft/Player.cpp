#include "pch.h"
#include "Player.h"

#include "Engine/DefaultResources.h"
#include "Engine/Utils.h"

using ButtonState = DirectX::Mouse::ButtonStateTracker::ButtonState;

Vector3 collisionPoints[] = {
	{ 0.3f, 0, 0 },
	{ -0.3f, 0, 0 },
	{ 0, 0, 0.3f },
	{ 0, 0, -0.3f },
	{ 0, -0.5f, 0 },
	{ 0.3f, 1.0f, 0 },
	{ -0.3f, 1.0f, 0 },
	{ 0, 1.0f, 0.3f },
	{ 0, 1.0f, -0.3f },
	{ 0, 1.5f, 0 },
};

void Player::GenerateGPUResources(DeviceResources* deviceRes) {
	currentCube.Generate(deviceRes);
	highlightCube.Generate(deviceRes);
}


void Player::Update(const float dt, const DirectX::Keyboard::State& kb, const DirectX::Mouse::State& ms) {
	keyboardTracker.Update(kb);
	mouseTracker.Update(ms);

	Vector3 delta;
	if (kb.Z)
		delta += Vector3::Forward;
	if (kb.S)
		delta += Vector3::Backward;
	if (kb.Q)
		delta += Vector3::Left;
	if (kb.D)
		delta += Vector3::Right;
	Vector3 move = Vector3::TransformNormal(delta, camera.GetInverseViewMatrix());
	move.y = 0.0;
	move.Normalize();
	position += move * walkSpeed * dt;

	Quaternion camRot = camera.GetRotation();
	camRot *= Quaternion::CreateFromAxisAngle(camera.Right(), -ms.y * dt * 0.25f);
	camRot *= Quaternion::CreateFromAxisAngle(Vector3::Up, -ms.x * dt * 0.25f);

	velocityY += -30 * dt;

	const Vector3 nextPos = position + Vector3(0, velocityY, 0) * dt;
	const auto* const downBlock = world->GetBlock(floor(nextPos.x + 0.5f), floor(nextPos.y), floor(nextPos.z + 0.5f));
	if (downBlock && *downBlock != EMPTY) {
		velocityY = -5 * dt;
		if (kb.Space)
			velocityY = 10.0f;
	}
	position += Vector3(0, velocityY, 0) * dt;

	for (auto colPoint : collisionPoints) {
		const Vector3 colPos = position + colPoint + Vector3(0.5f, 0.5f, 0.5f);

		const auto* block = world->GetBlock(floor(colPos.x), floor(colPos.y), floor(colPos.z));
		if (block && *block != EMPTY) {
			if (colPoint.x != 0)
				position.x += round(colPos.x) - colPos.x;
			if (colPoint.z != 0)
				position.z += round(colPos.z) - colPos.z;
			if (colPoint.y != 0 && colPoint.x == 0 && colPoint.z == 0)
				position.y += round(colPos.y) - colPos.y;
		}
	}

	camera.SetRotation(camRot);
	camera.SetPosition(position + Vector3(0, 1.25f, 0));
	highlightCube.model = Matrix::Identity;


	const auto cubes = Raycast(camera.GetPosition() + Vector3(0.5, 0.5, 0.5), camera.Forward(), 5);
	for (int i = 0; i < cubes.size(); i++) {
		const auto block = world->GetBlock(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (!block)
			continue;
		if (*block == EMPTY)
			continue;

		highlightCube.model = Matrix::CreateTranslation(cubes[i][0], cubes[i][1], cubes[i][2]);
		if (mouseTracker.leftButton == ButtonState::PRESSED) {
			world->UpdateBlock(cubes[i][0], cubes[i][1], cubes[i][2], EMPTY);
		}
		else if (mouseTracker.rightButton == ButtonState::PRESSED && i > 0) {
			world->UpdateBlock(cubes[i - 1][0], cubes[i - 1][1], cubes[i - 1][2], currentCube.GetBlockId());
		}
		break;
	}

	if (ms.scrollWheelValue != 0) {
		int id = currentCube.GetBlockId();
		id = (int)(id + signInt(ms.scrollWheelValue)) % COUNT;
		currentCube.SetBlockId((BlockId)id);
	}
}

void Player::Draw(DeviceResources* deviceRes) {
	const auto gpuRes = DefaultResources::Get();

	gpuRes->noDepth.Apply(deviceRes);
	gpuRes->cbModel.ApplyToVS(deviceRes, 0);
	const Matrix cubePos = Matrix::CreateTranslation(1.5, -1.5, -2) * camera.GetInverseViewMatrix();

	gpuRes->cbModel.UpdateBuffer(deviceRes, { cubePos.Transpose() });
	currentCube.Draw(deviceRes);

	gpuRes->depthEqual.Apply(deviceRes);
	gpuRes->cbModel.UpdateBuffer(deviceRes, { highlightCube.model.Transpose() });
	highlightCube.Draw(deviceRes);

	gpuRes->cbModel.UpdateBuffer(deviceRes, { Matrix::Identity });
	gpuRes->defaultDepth.Apply(deviceRes);
}
