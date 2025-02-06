#pragma once
#include "Block.h"
#include "Cube.h"

constexpr int WORLD_SIZE = 16;

class World {
public:
	BlockId* GetBlock(int gx, int gy, int gz);
	void Generate(const DeviceResources* deviceRes);
	void Draw(const DeviceResources* deviceRes);

private:
	std::vector<BlockId> blocks;
	std::vector<Cube> cubes;

	struct ModelData {
		Matrix modelMatrix;
	};

	ConstantBuffer<ModelData> cbModelData;
};
