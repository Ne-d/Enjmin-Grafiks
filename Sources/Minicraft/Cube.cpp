#include "pch.h"
#include "Cube.h"

Cube::Cube()
	:
	modelMatrix(Matrix::Identity) {
}

Cube::Cube(const Vector3 position)
	:
	modelMatrix(Matrix::CreateTranslation(position)) {
}

void Cube::Generate(const DeviceResources* deviceRes) {
	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right);
	PushFace({ 0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Left);

	PushFace({ 0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward);
	PushFace({ -0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Backward);

	PushFace({ 0.5f, 0.5f, 0.5f }, Vector3::Left, Vector3::Forward);
	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Right, Vector3::Forward);

	vertexBuffer.Create(deviceRes);
	indexBuffer.Create(deviceRes);
}

const VertexBuffer<VertexLayout_PositionUV>& Cube::GetVertexBuffer() const {
	return vertexBuffer;
}

const IndexBuffer& Cube::GetIndexBuffer() const {
	return indexBuffer;
}

const Matrix& Cube::GetModelMatrix() const {
	return modelMatrix;
}

void Cube::SetModelMatrix(const Matrix& modelMatrix) {
	this->modelMatrix = modelMatrix;
}

void Cube::Draw(const DeviceResources* deviceRes) const {
	vertexBuffer.Apply(deviceRes);
	indexBuffer.Apply(deviceRes);

	deviceRes->GetD3DDeviceContext()->DrawIndexed(indexBuffer.Size(), 0, 0);
}

Vector4 ToVector4(const Vector3 v3) {
	return Vector4(v3.x, v3.y, v3.z, 1.0f);
}

void Cube::PushFace(const Vector3 position, const Vector3 up, const Vector3 right) {
	// Vertex 0: Bottom-left
	const auto a = vertexBuffer.PushVertex({ ToVector4(position), { 0, 0 } });

	// Vertex 1: Top-left
	const auto b = vertexBuffer.PushVertex({ ToVector4(position + up), { 0, 1 } });

	// Vertex 2: Bottom-right
	const auto c = vertexBuffer.PushVertex({ ToVector4(position + right), { 1, 0 } });

	// Vertex 3: Top-right
	const auto d = vertexBuffer.PushVertex({ ToVector4(position + right + up), { 1, 0 } });

	// Indices
	indexBuffer.PushTriangle(a, b, c);
	indexBuffer.PushTriangle(c, b, d);
}
