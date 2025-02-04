#include "pch.h"
#include "Cube.h"

Cube::Cube() {
	PushFace(DirectX::SimpleMath::Vector3::Zero, DirectX::SimpleMath::Vector3::Up, DirectX::SimpleMath::Vector3::Right);
}

Cube::Cube(const DirectX::SimpleMath::Vector3 position) {
	PushFace(position, DirectX::SimpleMath::Vector3::Up, DirectX::SimpleMath::Vector3::Right);
}

std::vector<float> Cube::GetVertices() const {
	return vertices;
}

std::vector<uint32_t> Cube::GetIndices() const {
	return indices;
}

void Cube::PushFace(const DirectX::SimpleMath::Vector3 position, const DirectX::SimpleMath::Vector3 up,
					const DirectX::SimpleMath::Vector3 right) {
	// Vertex 0: Bottom-left
	vertices.emplace_back(position.x);
	vertices.emplace_back(position.y);
	vertices.emplace_back(position.z);

	// Vertex 1: Top-left
	vertices.emplace_back(position.x + up.x);
	vertices.emplace_back(position.y + up.y);
	vertices.emplace_back(position.z + up.z);

	// Vertex 2: Bottom-right
	vertices.emplace_back(position.x + right.x);
	vertices.emplace_back(position.y + right.y);
	vertices.emplace_back(position.z + right.z);

	// Vertex 3: Top-right
	vertices.emplace_back(position.x + right.x + up.x);
	vertices.emplace_back(position.y + right.y + up.y);
	vertices.emplace_back(position.z + right.z + up.z);

	// Indices
	indices.emplace_back(0);
	indices.emplace_back(1);
	indices.emplace_back(2);
	indices.emplace_back(1);
	indices.emplace_back(3);
	indices.emplace_back(2);
}
