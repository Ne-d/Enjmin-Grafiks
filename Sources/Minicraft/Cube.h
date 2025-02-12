#pragma once

#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"
#include "Minicraft/World.h"
#include "Minicraft/Block.h"

class Cube {
	BlockId blockId;

	VertexBuffer<VertexLayout_PositionNormalUV> vb;
	IndexBuffer ib;

	bool needRegen = true;

public:
	Matrix model = Matrix::Identity;

	explicit Cube(const BlockId id)
		:
		blockId(id) {
	}

	[[nodiscard]] BlockId GetBlockId() const { return blockId; }

	void SetBlockId(const BlockId& id) {
		blockId = id;
		needRegen = true;
	}

	void Generate(DeviceResources* deviceRes);
	void Draw(DeviceResources* deviceRes);

private:
	void PushFace(Vector3 pos, Vector3 up, Vector3 right, Vector3 normal, int id);
};
