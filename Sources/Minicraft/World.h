#pragma once
#include "Block.h"
#include "Chunk.h"

class World {
public:
	World(unsigned int sizeX, unsigned int sizeY, unsigned int sizeZ);
	
	BlockId* GetBlock(int gx, int gy, int gz);
	Chunk* GetChunk(int chunkX, int chunkY, int chunkZ);
	
	void Generate(const DeviceResources* deviceRes);
	void Draw(const DeviceResources* deviceRes);

private:
	// Methods
	void GenerateChunks(const DeviceResources* deviceRes);
	void GenerateChunk(const DeviceResources* deviceRes, unsigned int chunkX, unsigned int chunkY, unsigned int chunkZ);

	// Data
	unsigned int nbChunksX;
	unsigned int nbChunksY;
	unsigned int nbChunksZ;

	std::vector<Chunk> chunks;

	struct ModelData {
		Matrix modelMatrix;
	};

	ConstantBuffer<ModelData> cbModelData;
};
