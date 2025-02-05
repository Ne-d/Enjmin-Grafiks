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

// Tile coordinates from 0 to 16
Vector2 GetUvFromTexId(const int texId) {
	return Vector2(
		(float)(texId % ATLAS_NUMBER_OF_TILES) / ATLAS_NUMBER_OF_TILES,
		(float)(texId / ATLAS_NUMBER_OF_TILES) / ATLAS_NUMBER_OF_TILES
	);
}

void Cube::Generate(const DeviceResources* deviceRes) {
	const auto blockData = BlockData::Get(blockId);
	const auto sideFaceUv = GetUvFromTexId(blockData.texIdSide);
	const auto topFaceUv = GetUvFromTexId(blockData.texIdTop);
	const auto bottomFaceUv = GetUvFromTexId(blockData.texIdBottom);

	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Right, sideFaceUv);
	PushFace({ 0.5f, -0.5f, 0.5f }, Vector3::Up, Vector3::Forward, sideFaceUv);
	PushFace({ 0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Left, sideFaceUv);
	PushFace({ -0.5f, -0.5f, -0.5f }, Vector3::Up, Vector3::Backward, sideFaceUv);
	PushFace({ 0.5f, 0.5f, 0.5f }, Vector3::Left, Vector3::Forward, topFaceUv);
	PushFace({ -0.5f, -0.5f, 0.5f }, Vector3::Right, Vector3::Forward, bottomFaceUv);
	
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

void Cube::PushFace(const Vector3 position, const Vector3 up, const Vector3 right, const Vector2 uv) {
	constexpr float uvTileSize = 1.0 / 16.0;
	
	// Vertex 0: Bottom-left
	const auto a = vertexBuffer.PushVertex({
		ToVector4(position),
		{ uv.x, uv.y + uvTileSize },
	});

	// Vertex 1: Top-left
	const auto b = vertexBuffer.PushVertex({
		ToVector4(position + up),
		{ uv.x, uv.y }
	});

	// Vertex 2: Bottom-right
	const auto c = vertexBuffer.PushVertex({
		ToVector4(position + right),
		{ uv.x + uvTileSize, uv.y + uvTileSize },
	});

	// Vertex 3: Top-right
	const auto d = vertexBuffer.PushVertex({
		ToVector4(position + right + up),
		{ uv.x + uvTileSize, uv.y },
	});

	// Indices
	indexBuffer.PushTriangle(a, b, c);
	indexBuffer.PushTriangle(c, b, d);
}
