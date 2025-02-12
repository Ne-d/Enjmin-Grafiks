#pragma once

#include "Engine/Buffers.h"

using namespace DirectX::SimpleMath;

class Camera {
public:
	Camera(float fov, float aspectRatio);
	~Camera();

	void UpdateAspectRatio(float aspectRatio);
	void Update(float dt, const DirectX::Keyboard::State& keyboard, DirectX::Mouse* mouse);

	void Apply(const DeviceResources* deviceRes);

private:
	float fov;
	float nearPlane = 0.01f;
	float farPlane = 5000.0f;

	Vector3 position = Vector3(0, 0, 2);;
	Quaternion rotation = Quaternion::Identity;
	Matrix projection;
	Matrix view;

	struct MatrixData {
		Matrix mView;
		Matrix mProjection;
	};

	ConstantBuffer<MatrixData>* cbMatrices = nullptr;
};
