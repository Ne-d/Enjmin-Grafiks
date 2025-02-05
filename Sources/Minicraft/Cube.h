﻿#pragma once
#include "Block.h"
#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"

#define ATLAS_NUMBER_OF_TILES 16

class Cube {
public:
	explicit Cube();
	explicit Cube(Vector3 position, BlockId blockId = GRASS);

	void Generate(const DeviceResources* deviceRes);

	[[nodiscard]] const VertexBuffer<VertexLayout_PositionUV>& GetVertexBuffer() const;
	[[nodiscard]] const IndexBuffer& GetIndexBuffer() const;

	[[nodiscard]] const Matrix& GetModelMatrix() const;
	void SetModelMatrix(const Matrix& modelMatrix);

	void Draw(const DeviceResources* deviceRes) const;

private:
	// Methods
	void PushFace(Vector3 position, Vector3 up, Vector3 right, Vector2 uv);

	// Data
	VertexBuffer<VertexLayout_PositionUV> vertexBuffer;
	IndexBuffer indexBuffer;
	Matrix modelMatrix;

	BlockId blockId = DIRT;
};
