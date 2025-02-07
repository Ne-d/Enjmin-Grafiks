#include "pch.h"
#include "World.h"

World::World(const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ)
	:
	nbChunksX(sizeX),
	nbChunksY(sizeY),
	nbChunksZ(sizeZ) {
}

BlockId* World::GetBlock(const int gx, const int gy, const int gz) {
	// Check that coordinates are within bounds.
	if (gx < 0 || gy < 0 || gz < 0 ||
		gx >= nbChunksX * CHUNK_SIZE || gy >= nbChunksY * CHUNK_SIZE || gz >= nbChunksZ * CHUNK_SIZE) {
		return nullptr;
	}

	// Find the chunk containing this block
	const int chunkX = gx / CHUNK_SIZE;
	const int chunkY = gy / CHUNK_SIZE;
	const int chunkZ = gz / CHUNK_SIZE;
	auto* chunk = GetChunk(chunkX, chunkY, chunkZ);

	// Find the coordinates of the block within the chunk
	const int localX = gx % CHUNK_SIZE;
	const int localY = gy % CHUNK_SIZE;
	const int localZ = gz % CHUNK_SIZE;

	return chunk->GetBlock(localX, localY, localZ);
}

void World::SetBlock(const int gx, const int gy, const int gz, const BlockId block) {
	*GetBlock(gx, gy, gz) = block;
}

Chunk* World::GetChunk(const int chunkX, const int chunkY, const int chunkZ) {
	if (chunkX < 0 || chunkX >= nbChunksX ||
		chunkY < 0 || chunkY >= nbChunksY ||
		chunkZ < 0 || chunkZ >= nbChunksZ)
		return nullptr;

	return &chunks.at(chunkX + chunkY * nbChunksX + chunkZ * nbChunksX * nbChunksY);
}

void World::Generate(const DeviceResources* deviceRes) {
	InitializeChunks();
	GenerateChunks();
	GenerateCubes(deviceRes);

	cbModelData.Create(deviceRes);
}

void World::GenerateCubes(const DeviceResources* deviceRes) {
	for (unsigned int chunkX = 0; chunkX < nbChunksX; ++chunkX) {
		for (unsigned int chunkY = 0; chunkY < nbChunksY; ++chunkY) {
			for (unsigned int chunkZ = 0; chunkZ < nbChunksZ; ++chunkZ) {
				GetChunk(chunkX, chunkY, chunkZ)->GenerateCubes(deviceRes);
			}
		}
	}
}

void World::GenerateChunks() {
	// Generate block data
	for (int x = 0; x < nbChunksX * CHUNK_SIZE; ++x) {
		for (int y = 0; y < nbChunksY * CHUNK_SIZE; ++y) {
			for (int z = 0; z < nbChunksZ * CHUNK_SIZE; ++z) {
				constexpr int terrainHeight = 10;

				if (y < terrainHeight - 4)
					SetBlock(x, y, z, STONE);
				else if (y >= terrainHeight - 4 && y <= terrainHeight - 1)
					SetBlock(x, y, z, DIRT);
				else if (y == terrainHeight)
					SetBlock(x, y, z, GRASS);
			}
		}
	}
}

void World::InitializeChunks() {
	for (unsigned int chunkX = 0; chunkX < nbChunksX; ++chunkX) {
		for (unsigned int chunkY = 0; chunkY < nbChunksY; ++chunkY) {
			for (unsigned int chunkZ = 0; chunkZ < nbChunksZ; ++chunkZ) {
				auto& chunk = chunks.emplace_back(this, chunkX, chunkY, chunkZ);

				chunk.SetModelMatrix(Matrix::CreateTranslation(Vector3(chunkX * CHUNK_SIZE, chunkY * CHUNK_SIZE,
					chunkZ * CHUNK_SIZE)));

				chunk.GenerateBlocks();
			}
		}
	}
}

void World::Draw(const DeviceResources* deviceRes) {
	cbModelData.ApplyToVS(deviceRes, 0);

	for (auto chunk : chunks) {
		cbModelData.UpdateBuffer(deviceRes, { chunk.GetModelMatrix().Transpose() });
		chunk.Draw(deviceRes);
	}
}
