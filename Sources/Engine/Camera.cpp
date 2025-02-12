#include "pch.h"
#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera()
	:
	view(Matrix::CreateLookAt(position, position + Vector3::Forward, Vector3::Up)) {
	UpdateViewMatrix();
}

Camera::~Camera() {
	delete cbCameraMatrices;
	cbCameraMatrices = nullptr;
}

Vector3 Camera::GetPosition() const { return position; }

Quaternion Camera::GetRotation() const { return rotation; }

void Camera::SetPosition(const Vector3& position) {
	this->position = position;
	UpdateViewMatrix();
}

void Camera::SetRotation(const Quaternion& rotation) {
	this->rotation = rotation;
	UpdateViewMatrix();
}

Vector3 Camera::Forward() const { return Vector3::TransformNormal(Vector3::Forward, view.Invert()); }

Vector3 Camera::Up() const { return Vector3::TransformNormal(Vector3::Up, view.Invert()); }

Vector3 Camera::Right() const { return Vector3::TransformNormal(Vector3::Right, view.Invert()); }

void Camera::UpdateViewMatrix() {
	const Vector3 newForward = Vector3::Transform(Vector3::Forward, rotation);
	const Vector3 newUp = Vector3::Transform(Vector3::Up, rotation);

	view = Matrix::CreateLookAt(position, position + newForward, newUp);

	BoundingFrustum::CreateFromMatrix(frustum, projection, true);
	frustum.Transform(frustum, view.Invert());
}

void Camera::Apply(const DeviceResources* deviceRes) {
	if (cbCameraMatrices == nullptr) {
		cbCameraMatrices = new ConstantBuffer<MatrixData>();
		cbCameraMatrices->Create(deviceRes);
	}

	const MatrixData data = { view.Transpose(), projection.Transpose() };

	cbCameraMatrices->UpdateBuffer(deviceRes, data);
	cbCameraMatrices->ApplyToVS(deviceRes, 1);
}

PerspectiveCamera::PerspectiveCamera(const float fov, const float aspectRatio)
	:
	fov(fov) {
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}

void PerspectiveCamera::UpdateAspectRatio(float aspectRatio) {
	projection = Matrix::CreatePerspectiveFieldOfView(XMConvertToRadians(fov), aspectRatio, nearPlane, farPlane);
}
