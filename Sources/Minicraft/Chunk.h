#pragma once

#include "Block.h"
#include "Engine/Buffers.h"
#include "Engine/VertexLayout.h"

constexpr int ATLAS_NUMBER_OF_TILES = 16;
constexpr int CHUNK_SIZE = 16;

class World;

class Chunk {
public:
	explicit Chunk(World* world, int chunkX, int chunkY, int chunkZ);

	void GenerateBlocks();
	void GenerateCubes(const DeviceResources* deviceRes);

	[[nodiscard]] BlockId* GetBlock(int x, int y, int z);
	void SetBlock(int x, int y, int z, BlockId blockId);

	[[nodiscard]] const Matrix& GetModelMatrix() const;
	void SetModelMatrix(const Matrix& modelMatrix);

	void Draw(const DeviceResources* deviceRes, const RenderPass& renderPass) const;

private:
	// Methods
	void PushFace(Vector3 position, Vector3 up, Vector3 right, Vector2 uv,
				  RenderPass renderPass);
	void GenerateCube(Vector3 position, BlockId blockId);

	bool IsFaceVisible(Vector3 position, Vector3 direction);

	// Data
	int chunkX;
	int chunkY;
	int chunkZ;

	std::vector<BlockId> blocks;
	World* world;

	Matrix modelMatrix;

	std::vector<VertexBuffer<VertexLayout_PositionNormalUV>> vertexBuffers;
	std::vector<IndexBuffer> indexBuffers;
};
