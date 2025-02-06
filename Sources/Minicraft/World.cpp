#include "pch.h"
#include "World.h"

BlockId* World::GetBlock(const int gx, const int gy, const int gz) {
	const uint32_t index = gx + gy * WORLD_SIZE + gz * WORLD_SIZE * WORLD_SIZE;

	if (index < 0 || index > WORLD_SIZE * WORLD_SIZE * WORLD_SIZE)
		return nullptr;

	return &blocks.at(index);
}

void World::Generate(const DeviceResources* deviceRes) {
	blocks.assign(WORLD_SIZE * WORLD_SIZE * WORLD_SIZE, EMPTY);

	// Generate world data
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int z = 0; z < WORLD_SIZE; z++) {
			for (int y = 0; y < 3; y++) {
				auto* const block = GetBlock(x, y, z);
				if (block == nullptr)
					continue;
				*block = DIRT;
			}

			auto* const block = GetBlock(x, 3, z);
			if (block == nullptr)
				continue;

			*block = GRASS;
		}
	}

	// Create Cube objects from world data
	for (int x = 0; x < WORLD_SIZE; x++) {
		for (int y = 0; y < WORLD_SIZE; y++) {
			for (int z = 0; z < WORLD_SIZE; z++) {
				auto block = GetBlock(x, y, z);
				if (block == nullptr)
					continue;
				if (EMPTY == *block)
					continue;

				auto& cube = cubes.emplace_back(Vector3(x, y, z));
				cube.blockId = *block;
				cube.Generate(deviceRes);
			}
		}
	}

	cbModelData.Create(deviceRes);
}

void World::Draw(const DeviceResources* deviceRes) {
	cbModelData.ApplyToVS(deviceRes, 0);

	for (auto cube : cubes) {
		cbModelData.UpdateBuffer(deviceRes, { cube.GetModelMatrix().Transpose() });

		cube.Draw(deviceRes);
	}
}
