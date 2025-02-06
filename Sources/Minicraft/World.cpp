#include "pch.h"
#include "World.h"

World::World(const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ)
	:
	nbChunksX(sizeX),
	nbChunksY(sizeY),
	nbChunksZ(sizeZ) {
}

// TODO: Test this for the love of fuck
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
	const int blockX = gx % CHUNK_SIZE;
	const int blockY = gy % CHUNK_SIZE;
	const int blockZ = gz % CHUNK_SIZE;

	return chunk->GetBlock(blockX, blockY, blockZ);
}

Chunk* World::GetChunk(const int chunkX, const int chunkY, const int chunkZ) {
	if (chunkX < 0 || chunkX >= nbChunksX ||
		chunkY < 0 || chunkY >= nbChunksY ||
		chunkZ < 0 || chunkZ >= nbChunksZ)
		return nullptr;

	return &chunks.at(chunkX + chunkY * nbChunksX + chunkZ * nbChunksX * nbChunksY);
}

void World::Generate(const DeviceResources* deviceRes) {
	GenerateChunks(deviceRes);

	cbModelData.Create(deviceRes);
}

void World::GenerateChunks(const DeviceResources* deviceRes) {
	// Generate chunks from world data


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

	for (unsigned int chunkX = 0; chunkX < nbChunksX; ++chunkX) {
		for (unsigned int chunkY = 0; chunkY < nbChunksY; ++chunkY) {
			for (unsigned int chunkZ = 0; chunkZ < nbChunksZ; ++chunkZ) {
				GetChunk(chunkX, chunkY, chunkZ)->GenerateCubes(deviceRes);
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
