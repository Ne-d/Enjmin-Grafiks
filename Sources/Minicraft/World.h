#pragma once
#include "Block.h"
#include "Chunk.h"
#include "Engine/Camera.h"

class World {
public:
	World(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
	
	BlockId* GetBlock(int gx, int gy, int gz);
	void SetBlock(int gx, int gy, int gz, BlockId block);

	Chunk* GetChunk(int chunkX, int chunkY, int chunkZ);
	
	void Generate(const DeviceResources* deviceRes);
	void GenerateCubes(const DeviceResources* deviceRes);
	void Draw(Camera* camera, const DeviceResources* deviceRes, RenderPass renderPass);

	void UpdateBlock(int gx, int gy, int gz, BlockId newBlock);
	void MakeChunkDirty(int gx, int gy, int gz);

	friend class Chunk;

private:
	// Methods
	void InitializeChunks();
	void GenerateChunks();
	void GenerateChunk(const DeviceResources* deviceRes, unsigned int chunkX, unsigned int chunkY, unsigned int chunkZ);

	// Data
	unsigned int nbChunksX;
	unsigned int nbChunksY;
	unsigned int nbChunksZ;

	std::vector<std::vector<std::vector<Chunk>>> chunks;

	struct ModelData {
		Matrix modelMatrix;
	};

	ConstantBuffer<ModelData> cbModelData;
};
