#pragma once

#include "Engine/Buffers.h"

using namespace DirectX::SimpleMath;

class Camera {
public:
	Camera();
	virtual ~Camera();

	[[nodiscard]] Vector3 GetPosition() const;
	[[nodiscard]] Quaternion GetRotation() const;
	void SetPosition(const Vector3& position);
	void SetRotation(const Quaternion& rotation);

	[[nodiscard]] Vector3 Forward() const;
	[[nodiscard]] Vector3 Up() const;
	[[nodiscard]] Vector3 Right() const;

	[[nodiscard]] Matrix GetViewMatrix() const { return view; }
	[[nodiscard]] Matrix GetInverseViewMatrix() const { return view.Invert(); }

	void Apply(const DeviceResources* deviceRes);

	DirectX::BoundingFrustum frustum;

protected:
	// Data
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

	ConstantBuffer<MatrixData>* cbCameraMatrices = nullptr;

	// Methods
	void UpdateViewMatrix();
};


class PerspectiveCamera : public Camera {
public:
	PerspectiveCamera(float fov, float aspectRatio);

	void UpdateAspectRatio(float aspectRatio);

private:
	float fov;
};
