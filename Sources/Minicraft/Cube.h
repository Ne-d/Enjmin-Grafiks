#pragma once
#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"

class Cube {
public:
	explicit Cube();
	explicit Cube(Vector3 position);

	void Generate(const DeviceResources* deviceRes);

	[[nodiscard]] const VertexBuffer<VertexLayout_PositionUV>& GetVertexBuffer() const;
	[[nodiscard]] const IndexBuffer& GetIndexBuffer() const;

	[[nodiscard]] const Matrix& GetModelMatrix() const;
	void SetModelMatrix(const Matrix& modelMatrix);

	void Draw(const DeviceResources* deviceRes) const;

private:
	// Methods
	void PushFace(Vector3 position, Vector3 up, Vector3 right);

	// Data
	VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
	IndexBuffer indexBuffer;
	Matrix modelMatrix;
};
