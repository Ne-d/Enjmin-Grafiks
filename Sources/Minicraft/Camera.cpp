#include "pch.h"
#include "Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

Camera::Camera(const float fov, const float aspectRatio)
	:
	fov(XMConvertToRadians(fov)) {
	view = Matrix::CreateLookAt(position, position + Vector3::Forward, Vector3::Up);

	UpdateAspectRatio(aspectRatio);
}

Camera::~Camera() {
	delete cbMatrices;
	cbMatrices = nullptr;
}

void Camera::UpdateAspectRatio(const float aspectRatio) {
	projection = Matrix::CreatePerspectiveFieldOfView(fov, aspectRatio, nearPlane, farPlane);
}

void Camera::Update(float dt, const Keyboard::State& keyboard, DirectX::Mouse* mouse) {
	constexpr float rotationSpeed = 0.25f;
	constexpr float mouseSpeed = 10.0f;
	float speed = 15.0f;
	if (keyboard.LeftShift)
		speed *= 2.0f;

	const auto mouseState = mouse->GetState();
	const Matrix inverseView = view.Invert();

	Vector3 translationInput = Vector3::Zero;

	if (keyboard.D)
		translationInput += Vector3::Right;
	if (keyboard.Q)
		translationInput += Vector3::Left;
	if (keyboard.Z)
		translationInput += Vector3::Forward;
	if (keyboard.S)
		translationInput += Vector3::Backward;
	if (keyboard.E)
		translationInput += Vector3::Up;
	if (keyboard.A)
		translationInput += Vector3::Down;

	// TransformNormal(cameraSpaceVector, inverseView) turns the camera space vector into a world-space vector.
	Vector3 translationDelta = Vector3::TransformNormal(translationInput * speed, inverseView);
	position += translationDelta * dt;

	if (mouseState.positionMode == Mouse::MODE_RELATIVE) {
		const float dx = mouseState.x;
		const float dy = mouseState.y;
		if (mouseState.rightButton) {
			translationInput = Vector3(dx, -dy, 0.0f);
			translationDelta = Vector3::TransformNormal(translationInput * mouseSpeed, projection);
			position += translationDelta * dt;
		}
		else if (mouseState.leftButton) {
			Quaternion rotationDelta = Quaternion::Identity;
			rotationDelta *= Quaternion::CreateFromAxisAngle(Vector3::TransformNormal(Vector3::Right, inverseView),
				-dy * rotationSpeed * dt);
			rotationDelta *= Quaternion::CreateFromAxisAngle(Vector3::Up, -dx * rotationSpeed * dt);
			rotation *= rotationDelta;
		}
		else {
			mouse->SetMode(Mouse::MODE_ABSOLUTE);
		}
	}
	else if (mouseState.rightButton || mouseState.leftButton) {
		mouse->SetMode(Mouse::MODE_RELATIVE);
	}

	const Vector3 newForward = Vector3::Transform(Vector3::Forward, rotation);
	view = Matrix::CreateLookAt(position, position + newForward, Vector3::Up);
}

void Camera::Apply(const DeviceResources* deviceRes) {
	if (cbMatrices == nullptr) {
		cbMatrices = new ConstantBuffer<MatrixData>();
		cbMatrices->Create(deviceRes);
	}

	const MatrixData data = { view.Transpose(), projection.Transpose() };

	cbMatrices->UpdateBuffer(deviceRes, data);
	cbMatrices->ApplyToVS(deviceRes, 1);
}
