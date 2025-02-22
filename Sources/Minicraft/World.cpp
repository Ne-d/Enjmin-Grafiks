﻿#include "pch.h"
#include "World.h"

#include <PerlinNoise.hpp>

#include "Engine/Camera.h"
#include "Engine/DefaultResources.h"

World::World(const unsigned int sizeX, const unsigned int sizeY, const unsigned int sizeZ)
	:
	nbChunksX(sizeX),
	nbChunksY(sizeY),
	nbChunksZ(sizeZ) {
}

BlockId* World::GetBlock(const int gx, const int gy, const int gz) {
	// Check that coordinates are within world bounds.
	if (gx < 0 || gy < 0 || gz < 0 ||
		gx >= nbChunksX * CHUNK_SIZE ||
		gy >= nbChunksY * CHUNK_SIZE ||
		gz >= nbChunksZ * CHUNK_SIZE) {
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

	return &(chunks.at(chunkX).at(chunkY).at(chunkZ));
}

void World::Generate(const DeviceResources* deviceRes) {
	InitializeChunks();
	GenerateChunks(deviceRes);
	GenerateCubes(deviceRes);
}

void World::InitializeChunks() {
	for (unsigned int chunkX = 0; chunkX < nbChunksX; chunkX++) {
		chunks.push_back(std::vector<std::vector<Chunk>>());

		for (unsigned int chunkY = 0; chunkY < nbChunksY; chunkY++) {
			chunks.at(chunkX).push_back(std::vector<Chunk>());

			for (unsigned int chunkZ = 0; chunkZ < nbChunksZ; chunkZ++) {
				auto& chunk = chunks.at(chunkX).at(chunkY).emplace_back(this, chunkX, chunkY, chunkZ);

				chunk.SetModelMatrix(Matrix::CreateTranslation(Vector3(chunkX * CHUNK_SIZE, chunkY * CHUNK_SIZE,
					chunkZ * CHUNK_SIZE)));
			}
		}
	}
}

void World::GenerateChunks(const DeviceResources* deviceRes) {
	siv::BasicPerlinNoise<float> perlinNoise((int)'*');
	
	// Generate block data
	for (int x = 0; x < nbChunksX * CHUNK_SIZE; ++x) {
		for (int y = 0; y < nbChunksY * CHUNK_SIZE; ++y) {
			for (int z = 0; z < nbChunksZ * CHUNK_SIZE; ++z) {
				const auto fx = (float)x;
				const auto fy = (float)y;
				const auto fz = (float)z;

				constexpr float bigNoiseScale = 80;
				constexpr float bigNoiseAmplitude = 80;
				constexpr float bigNoiseOffsetX = 0;
				constexpr float bigNoiseOffsetY = 0;

				constexpr float mediumNoiseScale = 15;
				constexpr float mediumNoiseAmplitude = 8;
				constexpr float mediumNoiseOffsetX = 69;
				constexpr float mediumNoiseOffsetY = 420;

				constexpr float smallNoiseScale = 5;
				constexpr float smallNoiseAmplitude = 2;
				constexpr float smallNoiseOffsetX = 42;
				constexpr float smallNoiseOffsetY = 69420;

				const int terrainHeight = 10 +
					perlinNoise.octave2D_01(fx / bigNoiseScale + bigNoiseOffsetX,
						fz / bigNoiseScale + bigNoiseOffsetY, 1) * bigNoiseAmplitude

					+ perlinNoise.octave2D_01(fx / mediumNoiseScale + mediumNoiseOffsetX,
						fz / mediumNoiseScale + mediumNoiseOffsetY, 1) * mediumNoiseAmplitude

					+ perlinNoise.octave2D_01(fx / smallNoiseScale + smallNoiseOffsetX,
						fz / smallNoiseScale + smallNoiseOffsetY, 1) * smallNoiseAmplitude;

				constexpr int waterHeight = 48;
				constexpr int sandHeightDelta = 1;

				if (y < waterHeight)
					SetBlock(x, y, z, WATER);
				
				if (y < terrainHeight - 4)
					SetBlock(x, y, z, STONE);
				else if (y >= terrainHeight - 4 && y <= terrainHeight - 1)
					SetBlock(x, y, z, DIRT);
				else if (y == terrainHeight)
					SetBlock(x, y, z, GRASS);

				if (y >= waterHeight - sandHeightDelta && y < waterHeight + sandHeightDelta && y <= terrainHeight)
					SetBlock(x, y, z, SAND);
			}
		}
	}

	DefaultResources::Get()->cbModel.Create(deviceRes);
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

void World::Draw(const Camera* camera, const DeviceResources* deviceRes, const RenderPass renderPass) {
	DefaultResources::Get()->cbModel.ApplyToVS(deviceRes, 0);
	
	for (unsigned int chunkX = 0; chunkX < nbChunksX; ++chunkX) {
		for (unsigned int chunkY = 0; chunkY < nbChunksY; ++chunkY) {
			for (unsigned int chunkZ = 0; chunkZ < nbChunksZ; ++chunkZ) {
				auto& chunk = chunks.at(chunkX).at(chunkY).at(chunkZ);

				if (chunk.needRegen)
					chunk.GenerateCubes(deviceRes);

				if (chunk.bounds.Intersects(camera->frustum)) {
					DefaultResources::Get()->cbModel.UpdateBuffer(deviceRes, { chunk.GetModelMatrix().Transpose() });
					chunk.Draw(deviceRes, renderPass);
				}
			}
		}
	}

	DefaultResources::Get()->cbModel.UpdateBuffer(deviceRes, { Matrix::Identity });
}

void World::UpdateBlock(const int gx, const int gy, const int gz, const BlockId newBlock) {
	auto* block = GetBlock(gx, gy, gz);
	if (block == nullptr)
		return;
	*block = newBlock;

	MakeChunkDirty(gx, gy, gz);
	MakeChunkDirty(gx + 1, gy, gz);
	MakeChunkDirty(gx - 1, gy, gz);
	MakeChunkDirty(gx, gy + 1, gz);
	MakeChunkDirty(gx, gy - 1, gz);
	MakeChunkDirty(gx, gy, gz + 1);
	MakeChunkDirty(gx, gy, gz - 1);
}

void World::MakeChunkDirty(int gx, int gy, int gz) {
	auto* chunk = GetChunk(gx, gy, gz);
	if (chunk != nullptr)
		chunk->needRegen = true;
}
