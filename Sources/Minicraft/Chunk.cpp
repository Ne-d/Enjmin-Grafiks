#include "pch.h"
#include "Chunk.h"
#include "World.h"

Chunk::Chunk(World* world, const int chunkX, const int chunkY, const int chunkZ)
	:
	chunkX(chunkX),
	chunkY(chunkY),
	chunkZ(chunkZ),
	world(world),
	modelMatrix(Matrix::CreateTranslation(Vector3(chunkX, chunkY, chunkZ))) {
}

void Chunk::GenerateBlocks() {
	blocks.assign(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE, EMPTY);
}

void Chunk::GenerateCubes(const DeviceResources* deviceRes) {
	// Generate cubes from block data
	for (int x = 0; x < CHUNK_SIZE; ++x) {
		for (int y = 0; y < CHUNK_SIZE; ++y) {
			for (int z = 0; z < CHUNK_SIZE; ++z) {
				GenerateCube(Vector3(x, y, z), *GetBlock(x, y, z));
			}
		}
	}
	
	vertexBuffer.Create(deviceRes);
	indexBuffer.Create(deviceRes);
}


// Tile coordinates from 0 to 16
Vector2 GetUvFromTexId(const int texId) {
	return Vector2(
		(float)(texId % ATLAS_NUMBER_OF_TILES) / ATLAS_NUMBER_OF_TILES,
		(float)(texId / ATLAS_NUMBER_OF_TILES) / ATLAS_NUMBER_OF_TILES
	);
}

void Chunk::GenerateCube(const Vector3 position, const BlockId blockId) {
	if (blockId == EMPTY)
		return;
	
	const auto blockData = BlockData::Get(blockId);
	const auto sideFaceUv = GetUvFromTexId(blockData.texIdSide);
	const auto topFaceUv = GetUvFromTexId(blockData.texIdTop);
	const auto bottomFaceUv = GetUvFromTexId(blockData.texIdBottom);

	if (IsFaceVisible(position, Vector3::Backward))
		PushFace(position + Vector3(-0.5f, -0.5f, 0.5f), Vector3::Up, Vector3::Right, sideFaceUv);

	if (IsFaceVisible(position, Vector3::Right))
		PushFace(position + Vector3(0.5f, -0.5f, 0.5f), Vector3::Up, Vector3::Forward, sideFaceUv);

	if (IsFaceVisible(position, Vector3::Forward))
		PushFace(position + Vector3(0.5f, -0.5f, -0.5f), Vector3::Up, Vector3::Left, sideFaceUv);

	if (IsFaceVisible(position, Vector3::Left))
		PushFace(position + Vector3(-0.5f, -0.5f, -0.5f), Vector3::Up, Vector3::Backward, sideFaceUv);

	if (IsFaceVisible(position, Vector3::Up))
		PushFace(position + Vector3(0.5f, 0.5f, 0.5f), Vector3::Left, Vector3::Forward, topFaceUv);

	if (IsFaceVisible(position, Vector3::Down))
		PushFace(position + Vector3(-0.5f, -0.5f, 0.5f), Vector3::Right, Vector3::Forward, bottomFaceUv);
}

bool Chunk::IsFaceVisible(const Vector3 position, const Vector3 direction) {
	assert(*GetBlock(position.x, position.y, position.z) != EMPTY);
	assert(position.x >= 0 && position.x < CHUNK_SIZE &&
		position.y >= 0 && position.y < CHUNK_SIZE &&
		position.z >= 0 && position.z < CHUNK_SIZE);

	const auto positionToCheck = position + direction;

	BlockId* neighbour = nullptr;

	if (positionToCheck.x >= 0 && positionToCheck.x < CHUNK_SIZE &&
		positionToCheck.y >= 0 && positionToCheck.y < CHUNK_SIZE &&
		positionToCheck.z >= 0 && positionToCheck.z < CHUNK_SIZE) {
		neighbour = GetBlock(positionToCheck.x, positionToCheck.y, positionToCheck.z);
	}

	// If the block is empty, the face is visible.
	neighbour = world->GetBlock(
		chunkX * CHUNK_SIZE + positionToCheck.x,
		chunkY * CHUNK_SIZE + positionToCheck.y,
		chunkZ * CHUNK_SIZE + positionToCheck.z
	);

	if (neighbour == nullptr)
		return true;

	if (*neighbour == EMPTY)
		return true;

	return false;
}

const Matrix& Chunk::GetModelMatrix() const {
	return modelMatrix;
}

BlockId* Chunk::GetBlock(const int x, const int y, const int z) {
	if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE)
		return nullptr;

	return &blocks.at(x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE);
}

void Chunk::SetBlock(const int x, const int y, const int z, const BlockId blockId) {
	blocks.at(x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE) = blockId;
}

void Chunk::SetModelMatrix(const Matrix& modelMatrix) {
	this->modelMatrix = modelMatrix;
}

void Chunk::Draw(const DeviceResources* deviceRes) const {
	if (blocks.size() == 0)
		return;
	
	vertexBuffer.Apply(deviceRes);
	indexBuffer.Apply(deviceRes);

	deviceRes->GetD3DDeviceContext()->DrawIndexed(indexBuffer.Size(), 0, 0);
}

Vector4 ToVector4(const Vector3 v3) {
	return Vector4(v3.x, v3.y, v3.z, 1.0f);
}

void Chunk::PushFace(const Vector3 position, const Vector3 up, const Vector3 right, const Vector2 uv) {
	constexpr float uvTileSize = 1.0 / 16.0;
	
	// Vertex 0: Bottom-left
	const auto a = vertexBuffer.PushVertex({
		ToVector4(position),
		{ uv.x, uv.y + uvTileSize },
	});

	// Vertex 1: Top-left
	const auto b = vertexBuffer.PushVertex({
		ToVector4(position + up),
		{ uv.x, uv.y }
	});

	// Vertex 2: Bottom-right
	const auto c = vertexBuffer.PushVertex({
		ToVector4(position + right),
		{ uv.x + uvTileSize, uv.y + uvTileSize },
	});

	// Vertex 3: Top-right
	const auto d = vertexBuffer.PushVertex({
		ToVector4(position + right + up),
		{ uv.x + uvTileSize, uv.y },
	});

	// Indices
	indexBuffer.PushTriangle(a, b, c);
	indexBuffer.PushTriangle(c, b, d);
}
